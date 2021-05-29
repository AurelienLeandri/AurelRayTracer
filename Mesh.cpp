#include "Mesh.h"

#include "Transform.h"

void Mesh::transform(const Transform& t)
{
	for (Vertex& v : geometry) {
		v.normal = glm::vec3(glm::transpose(t.getInvMatrix()) * glm::vec4(v.normal, 0));
		v.position = glm::vec3(t.getMatrix() * glm::vec4(v.position, 1));
		int a = 0;
		a = a;
	}
}
