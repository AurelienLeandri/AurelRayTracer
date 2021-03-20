#include "Mesh.h"

#include "Transform.h"

void Mesh::transform(const Transform& t)
{
	// NOTE: glm::mat4 are column-major. The first index is the column number.

	glm::mat4 model(1);
	glm::mat4 translation(1.f);
	glm::mat4 scaling(1.f);
	for (int i = 0; i < 3; ++i) {
		scaling[i][i] = t.scaling[i];
		translation[3][i] = t.translation[i];
	}

	model = scaling * model;

	float sin_x = glm::sin(t.rotation_rads.x);
	float cos_x = glm::cos(t.rotation_rads.x);
	float sin_y = glm::sin(t.rotation_rads.y);
	float cos_y = glm::cos(t.rotation_rads.y);
	float sin_z = glm::sin(t.rotation_rads.z);
	float cos_z = glm::cos(t.rotation_rads.z);
	glm::mat4 rotation_x(1.f);
	rotation_x[1][1] = cos_x;
	rotation_x[2][1] = -sin_x;
	rotation_x[1][2] = sin_x;
	rotation_x[2][2] = cos_x;

	model = rotation_x * model;

	glm::mat4 rotation_y(1.f);
	rotation_y[0][0] = cos_y;
	rotation_y[2][0] = sin_y;
	rotation_y[0][2] = -sin_y;
	rotation_y[2][2] = cos_y;

	model = rotation_y * model;

	glm::mat4 rotation_z(1.f);
	rotation_z[0][0] = cos_z;
	rotation_z[1][0] = -sin_z;
	rotation_z[0][1] = sin_z;
	rotation_z[1][1] = cos_z;

	model = rotation_z * model;

	model = translation * model;

	for (Vertex& v : geometry) {
		v.normal = glm::vec3(glm::transpose(glm::inverse(model)) * glm::vec4(v.normal, 0));
		v.position = glm::vec3(model * glm::vec4(v.position, 1));
		int a = 0;
		a = a;
	}
}
