
#include <algorithm>

#include "Mesh.h"

bool operator==(const Vertex& v1, const Vertex& v2) 
{
	return v1.m_position == v2.m_position;
}

Mesh::Mesh(const Heightmap& heightmap, float texAspectRatio, int texRepeats)
{
	set(heightmap, texAspectRatio, texRepeats);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, float texAspectRatio, int texRepeats)
{
	set(vertices, indices, texAspectRatio, texRepeats);
}

void Mesh::computeNormals()
{
	for (Vertex& vertex : m_vertices)
    {
		vertex.m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    for (int i = 0; i < m_indices.size(); i += 3)
    {
        glm::vec3 a = m_vertices[m_indices[i]].m_position;
        glm::vec3 b = m_vertices[m_indices[i+1]].m_position;
        glm::vec3 c = m_vertices[m_indices[i+2]].m_position;

        glm::vec3 normal = glm::cross(b - a, c - a);

        m_vertices[m_indices[i]].m_normal += normal;
        m_vertices[m_indices[i+1]].m_normal += normal;
        m_vertices[m_indices[i+2]].m_normal += normal;
    }

    for (Vertex& vertex : m_vertices)
    {
		vertex.m_normal = glm::normalize(vertex.m_normal);
    }
}

void Mesh::setHeight(float height)
{
	if (m_vertices.empty() || height <= 0.0f || height == m_bbox.m_size.y)
	{
		return;
	}

	float scale = height / m_bbox.m_size.y;

	for (Vertex& vertex : m_vertices)
	{
		vertex.m_position.y *= scale;
	}

	m_bbox.compute(*this);

	computeNormals();
}

void Mesh::setTexCoords(float texAspectRatio, int texRepeats)
{
	if (m_vertices.empty() || texAspectRatio <= 0.0f || texRepeats < 1)
	{
		return;
	}

    float texLengthX = (m_bbox.m_size.x / texRepeats);
	float texLengthZ = (1 / texAspectRatio) * texLengthX;

    for (Vertex& vertex : m_vertices)
    {
		vertex.m_texCoords.x = (vertex.m_position.x - m_bbox.m_min.x) / texLengthX;
		vertex.m_texCoords.y = (vertex.m_position.y - m_bbox.m_min.y) / texLengthX;
		vertex.m_texCoords.z = (m_bbox.m_max.z - vertex.m_position.z) / texLengthZ;
    }
}

bool Mesh::set(const Heightmap& heightmap, float texAspectRatio, int texRepeats)
{
	if (heightmap.isEmpty() || texAspectRatio <= 0.0f || texRepeats < 1)
	{
        return false;
	}

	int heightmapWidth = heightmap.getWidth();
    int heightmapHeight = heightmap.getHeight();
	
	m_bbox.m_size.x = heightmapWidth - 1;
	m_bbox.m_size.y = heightmapWidth * 0.25f;
	m_bbox.m_size.z = heightmapHeight - 1;

	float scale = m_bbox.m_size.y / (heightmap.getMax() - heightmap.getMin());
	
	glm::vec3 start;
	start.x = -(heightmapWidth - 1) / 2.0f;
	start.y = -m_bbox.m_size.y / 2.0f;
	start.z = -(heightmapHeight - 1) / 2.0f;
    
	m_vertices.clear();
	m_vertices.reserve(heightmapWidth * heightmapHeight);
	for (int row = 0; row < heightmapHeight; ++row)
	{
		for (int col = 0; col < heightmapWidth; ++col)
		{
			Vertex vertex;
			vertex.m_position[0] = start.x + col; //x
			vertex.m_position[1] = start.y + (heightmap.at(row, col) * scale); //y
			vertex.m_position[2] = start.z + row; //z
			m_vertices.push_back(vertex);
		}
	}

	m_bbox.compute(*this);

    setTexCoords(texAspectRatio, texRepeats);

	//create 2 triangles for every point in the heightmap except for points in the last row and the last column

	m_indices.clear();
	m_indices.reserve(6 * (heightmapWidth - 1) * (heightmapHeight - 1));
	for (int row = 0; row < heightmapHeight - 1; ++row)
	{
		for (int col = 0; col < heightmapWidth - 1; ++col)
		{
			//first triangle
			m_indices.push_back((row * heightmapWidth) + col);
			m_indices.push_back(((row + 1) * heightmapWidth) + col);
			m_indices.push_back(((row + 1) * heightmapWidth) + (col + 1));

			//second triangle
			m_indices.push_back((row * heightmapWidth) + col);
			m_indices.push_back(((row + 1) * heightmapWidth) + (col + 1));
			m_indices.push_back((row * heightmapWidth) + (col + 1));
		}
	}

    computeNormals();

    return true;
}

bool Mesh::set(const std::vector<Vertex>& vertices, const std::vector<int>& indices, float texAspectRatio, int texRepeats)
{
	if (vertices.empty() || indices.empty() || texAspectRatio <= 0.0f || texRepeats < 1)
	{
		return false;
	}

	m_vertices = vertices;
	m_indices = indices;

	m_bbox.compute(*this);

	computeNormals();

	setTexCoords(texAspectRatio, texRepeats);

	return true;
}

Mesh Mesh::get(const Heightmap& heightmap, float texAspectRatio, int texRepeats)
{
	return Mesh(heightmap, texAspectRatio, texRepeats);
}

Mesh Mesh::get(const std::vector<Vertex>& vertices, const std::vector<int>& indices, float texAspectRatio, int texRepeats)
{
	return Mesh(vertices, indices, texAspectRatio, texRepeats);
}

const std::vector<Vertex>& Mesh::getVertices() const
{
	return m_vertices;
}

const std::vector<int>& Mesh::getIndices() const
{
	return m_indices;
}

const AABB& Mesh::getBoundingBox() const
{
	return m_bbox;
}

bool Mesh::empty() const
{
	return m_vertices.empty();
}
