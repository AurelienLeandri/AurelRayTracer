#include "Triangle.h"

#include "AABB.h"

Triangle::Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, std::shared_ptr<Material> material,
	const glm::vec3& translation, const glm::vec3& rotation) : Hitable(material)
{
	_vertices[0] = vertex0;
	_vertices[1] = vertex1;
	_vertices[2] = vertex2;
	transform(translation, rotation);
}

const Vertex& Triangle::operator[](int index) const
{
	return _vertices[index];
}

bool Triangle::_oldAlgo(Ray r, float t_min, float t_max, HitRecord& record) const {
	/*
	We first compute the triangle's plane equation Ax + By + Cz + D = 0, ABC being the normal, D begin computed from one of the vertices.
	The solution P is on the ray, so P = O + V * t, t unknown.
	P is also on the plane so AP_x + BP_y + CP_z + D = 0
	So A(O_x + V_xt) + B(O_y + V_yt) + C(O_z + V_zt) + D = 0
	t(AV_x + BV_y + CV_z) + AO_x + BO_y + CO_z + D = 0
	t(dot(N, V)) + dot(N, O) + D = 0
	t = -(dot(N, O) + D) / dot(N, V)
	/!\ dot(N, V) can be 0, this is the case when the ray and the plane are parallel (the direction vector V and the plane normal N are perp.)

	Then we need to make sure P is inside the triangle.
	An observation is that if it is left to each edge of the triangle, then it is inside.
	To know this, we compute for each edge the cross product R = (v1 - v0) X (P - v0), v1, v0 being the two vertices of an edge.
	This gives a vector perpendicular to the two others, having the same direction than the normal.
	Then from right hand rule, R and the normal must have the same orientation. If they go opposite ways, the point was on the right of the edge.
	To know this we simply check that the dot product of R and the normal is positive.
	If the dot product is positive for each vertex, then the point is inside the triangle.
	(NOTE: v0, v1, v2 need to be declared counter-clockwise or the point will be RIGHT of each edge)
	*/

	glm::vec3 triangle_normal = _normal;

	/*
	if (glm::dot(triangle_normal, r.direction) < 0)  // Triangle is back-facing, we make it front-facing
		triangle_normal *= -1;
	*/

	float normal_dot_direction = glm::dot(triangle_normal, r.direction);
	if (normal_dot_direction >= 0)  // the ray direction and the triangle plane are parallel
		return false;

	record.t = -(glm::dot(triangle_normal, r.origin) + _D) / normal_dot_direction;

	if (record.t <= 0)  // The triangle is behind the camera
		return false;

	if (record.t < t_min || record.t >= t_max)
		return false;

	record.position = r.direction * record.t + r.origin;
	const glm::vec3& P = record.position;  // just an alias

	// Is the point inside the triangle?
	glm::vec3 v0_P = P - v0.position;
	glm::vec3 v1_P = P - v1.position;
	glm::vec3 v2_P = P - v2.position;
	float dot0 = glm::dot(triangle_normal, glm::cross(v1.position - v0.position, v0_P));
	float dot1 = glm::dot(triangle_normal, glm::cross(v2.position - v1.position, v1_P));
	float dot2 = glm::dot(triangle_normal, glm::cross(v0.position - v2.position, v2_P));
	if (!((dot0 <= 0 && dot1 < 0 && dot2 <= 0) || (dot0 > 0 && dot1 > 0 && dot2 > 0)))  // all dots must have the same sign
	{
		return false;
	}

	// Some values for linear interpolation
	float length_v0_P = glm::length(v0_P);
	float length_v1_P = glm::length(v1_P);
	float length_v2_P = glm::length(v2_P);
	float inv_sum_lengths = 1 / (length_v0_P + length_v1_P + length_v2_P);

	// Compute normal at intersection point
	record.normal = glm::normalize(
		(length_v0_P * v0.normal + length_v1_P * v1.normal + length_v2_P * v2.normal) * inv_sum_lengths
	);

	// Compute UV-coordinates at intersection point
	glm::vec2 uv = (length_v0_P * v0.uv + length_v1_P * v1.uv + length_v2_P * v2.uv) * inv_sum_lengths;
	record.u = uv.x;
	record.v = uv.y;

	/*
	record.material = material.get();
	if (material) {
		material->getBSDF(record);
		material->emit(record);
	}
	*/

	return true;
}

void Triangle::transform(const glm::vec3& translation, const glm::vec3& rotation)
{
	glm::mat4 rotation_mat_x = glm::mat4();
	glm::mat4 rotation_mat_y = glm::mat4();
	glm::mat4 rotation_mat_z = glm::mat4();
	glm::mat4 translation_mat = glm::mat4();
	for (int i = 0; i < 4; ++i) {
		rotation_mat_x[i][i] = 1;
		rotation_mat_y[i][i] = 1;
		rotation_mat_z[i][i] = 1;
		translation_mat[i][i] = 1;
	}
	translation_mat[3][0] = translation.x;
	translation_mat[3][1] = translation.y;
	translation_mat[3][2] = translation.z;
	float cos_x = glm::cos(rotation.x);
	float cos_y = glm::cos(rotation.y);
	float cos_z = glm::cos(rotation.z);
	float sin_x = glm::sin(rotation.x);
	float sin_y = glm::sin(rotation.y);
	float sin_z = glm::sin(rotation.z);
	rotation_mat_x[1][1] = cos_x;
	rotation_mat_x[1][2] = -sin_x;
	rotation_mat_x[2][1] = sin_x;
	rotation_mat_x[2][2] = cos_x;

	rotation_mat_y[0][0] = cos_y;
	rotation_mat_y[0][2] = sin_y;
	rotation_mat_y[2][0] = -sin_y;
	rotation_mat_y[2][2] = cos_y;

	rotation_mat_z[0][0] = cos_z;
	rotation_mat_z[0][1] = -sin_z;
	rotation_mat_z[1][0] = sin_z;
	rotation_mat_z[1][1] = cos_z;
	for (int i = 0; i < 3; ++i) {
		Vertex& vertex = this->_vertices[i];
		vertex.position = glm::vec3(translation_mat * rotation_mat_x * rotation_mat_y * rotation_mat_z * glm::vec4(vertex.position, 1.0));
		vertex.normal = glm::normalize(glm::vec3(rotation_mat_x * rotation_mat_y * rotation_mat_z * glm::vec4(vertex.normal, 0.0)));
	}
	_computeTriangleNormal();
	_computePlaneDParameter();
}

bool Triangle::hit(Ray r, float t_min, float t_max, HitRecord& record) const
{
	return _oldAlgo(r, t_min, t_max, record);

	/*
	float normal_dot_direction = glm::dot(_normal, r.direction);
	if (normal_dot_direction >= 0)  // the ray direction and the triangle plane are parallel
		return false;

	static const float EPSILON = 0.000001f;
	glm::vec3 edge1, edge2, tvec, pvec, qvec;
	float det, inv_det;
	float& t = record.t;
	float& u = record.u;
	float& v = record.v;

	// Find vectors for two edges sharing vertex 0
	edge1 = v1.position - v0.position;
	edge2 = v2.position - v0.position;

	// Calculate determinant (and later U parameter)
	pvec = glm::cross(r.direction, edge2);

	// If determinant is near zero, ray lies in plane of triangle
	det = glm::dot(edge1, pvec);

	// No backface culling
	if (det > -EPSILON && det < EPSILON)
		return false;
	inv_det = 1 / det;

	// Calculate distance from v0 to ray origin
	tvec = r.origin - v0.position;

	// Calculate U parameter and test bounds
	u = glm::dot(tvec, pvec) * inv_det;
	if (u < 0.f || u > 1.f)
		return false;

	// Prepare to test V parameter
	qvec = glm::cross(tvec, edge1);

	// Calculate V parameter and test bounds
	v = glm::dot(r.direction, qvec) * inv_det;
	if (v < 0.f || u + v > 1.f)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = glm::dot(edge2, qvec) * inv_det;

	// Some values for linear interpolation
	record.position = r.direction * record.t + r.origin;
	float length_v0_P = glm::length(record.position - v0.position);
	float length_v1_P = glm::length(record.position - v1.position);
	float length_v2_P = glm::length(record.position - v2.position);
	float inv_sum_lengths = 1 / (length_v0_P + length_v1_P + length_v2_P);

	// Compute normal at intersection point
	record.normal = glm::normalize(
		(length_v0_P * v0.normal + length_v1_P * v1.normal + length_v2_P * v2.normal) * inv_sum_lengths
	);

	record.normal = v0.normal;


	record.material = material.get();
	if (material) {
		material->getBSDF(record);
		material->emit(record);
	}

	return true;
	*/
}

bool Triangle::boundingBox(float t0, float t1, AABB& box) const
{
	glm::vec3 min_vec;
	glm::vec3 max_vec;
	int min_distance_index = 0;
	float min_distance = std::numeric_limits<float>::max();
	int max_distance_index = 0;
	float max_distance = std::numeric_limits<float>::min();
	for (int i = 0; i < 3; ++i) {
		min_vec[i] = glm::min(glm::min(_vertices[0].position[i], _vertices[1].position[i]), _vertices[2].position[i]);
		max_vec[i] = glm::max(glm::max(_vertices[0].position[i], _vertices[1].position[i]), _vertices[2].position[i]);
		float candidate_distance = max_vec[i] - min_vec[i];
		if (candidate_distance < min_distance) {
			min_distance = candidate_distance;
			min_distance_index = i;
		}
		if (candidate_distance > max_distance) {
			max_distance = candidate_distance;
			max_distance_index = i;
		}
	}
	max_distance /= 2;
	max_vec[min_distance_index] += max_distance;  // Just to be sure the box has non zero area
	min_vec[min_distance_index] -= max_distance;  // Just to be sure the box has non zero area
	box = AABB(min_vec, max_vec);
	return true;
}

void Triangle::_computeTriangleNormal()
{
	_normal = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
}

void Triangle::_computePlaneDParameter()
{
	_D = -glm::dot(this->v0.position, _normal);
}
