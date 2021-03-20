#include "Mesh.h"

void Mesh::transform(const Transformation& t)
{
	glm::mat4 translation_scaling(1.f);
	for (int i = 0; i < 3; ++i) {
		translation_scaling[i][i] = t.scaling[i];
		translation_scaling[i][3] = t.translation[i];
	}

	float sin_x = glm::sin(t.rotation_rads.x);
	float cos_x = glm::cos(t.rotation_rads.x);
	float sin_y = glm::sin(t.rotation_rads.y);
	float cos_y = glm::cos(t.rotation_rads.y);
	float sin_z = glm::sin(t.rotation_rads.z);
	float cos_z = glm::cos(t.rotation_rads.z);
	glm::mat4 rotation_x(1.f);
	rotation_x[1][1] = cos_x;
	rotation_x[1][2] = -sin_x;
	rotation_x[2][1] = sin_x;
	rotation_x[2][2] = cos_x;
	glm::mat4 rotation_y(1.f);
	rotation_y[0][0] = cos_y;
	rotation_y[0][2] = sin_y;
	rotation_y[2][0] = -sin_y;
	rotation_y[2][2] = cos_y;
	glm::mat4 rotation_z(1.f);
	rotation_z[0][0] = cos_z;
	rotation_z[0][1] = -sin_z;
	rotation_z[1][0] = sin_z;
	rotation_z[1][1] = cos_z;
}
