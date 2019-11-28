
#include "AABB.h"
#include "Mesh.h"

#include <algorithm> 
#include <limits>
#include <vector>

AABB::AABB(const Mesh& mesh)
{
	compute(mesh);
}

void AABB::compute(const Mesh& mesh)
{
	const std::vector<Vertex>& vertices = mesh.getVertices();
	
	if (vertices.empty())
	{
		m_min = glm::vec3(0.0f);
	    m_max = glm::vec3(0.0f);
		m_size = glm::vec3(0.0f);

		return;
	}

	m_min = glm::vec3(std::numeric_limits<float>::max());
	m_max = glm::vec3(std::numeric_limits<float>::min());
	
	for (const Vertex& vertex : vertices)
	{
		m_min.x = std::min(m_min.x, vertex.m_position.x);
		m_min.y = std::min(m_min.y, vertex.m_position.y);
		m_min.z = std::min(m_min.z, vertex.m_position.z);

		m_max.x = std::max(m_max.x, vertex.m_position.x);
		m_max.y = std::max(m_max.y, vertex.m_position.y);
		m_max.z = std::max(m_max.z, vertex.m_position.z);
	}

	m_size = m_max - m_min;
}

float AABB::getLongestSide() const
{
	return std::max({ m_size.x, m_size.y, m_size.z });
}
