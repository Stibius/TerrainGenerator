#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "AABB.h"
#include "Heightmap.h"

struct Vertex
{
	glm::vec3 m_position;
	glm::vec3 m_texCoords;
	glm::vec3 m_normal;
};

bool operator==(const Vertex& v1, const Vertex& v2);

class Mesh 
{

public:

	Mesh() = default;

	explicit Mesh(const Heightmap& heightmap, float texAspectRatio = 1.0f, int texRepeats = 1);

	Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, float texAspectRatio = 1.0f, int texRepeats = 1);

	Mesh(const Mesh& other) = default;

	Mesh(Mesh&& other) = default;

	Mesh& operator=(const Mesh& other) = default;

	Mesh& operator=(Mesh&& other) = default;

	~Mesh() = default;
	
	bool set(const Heightmap& heightmap, float texAspectRatio = 1.0f, int texRepeats = 1);

	bool set(const std::vector<Vertex>& vertices, const std::vector<int>& indices, float texAspectRatio = 1.0f, int texRepeats = 1);

	static Mesh get(const Heightmap& heightmap, float texAspectRatio = 1.0f, int texRepeats = 1);

	static Mesh get(const std::vector<Vertex>& vertices, const std::vector<int>& indices, float texAspectRatio = 1.0f, int texRepeats = 1);

	void setHeight(float height);

    void setTexCoords(float texAspectRatio, int texRepeats);

	const std::vector<Vertex>& getVertices() const;

	const std::vector<int>& getIndices() const;

	const AABB& getBoundingBox() const;

	bool empty() const;

private:

	std::vector<Vertex> m_vertices;
	std::vector<int> m_indices; 
	AABB m_bbox;

	void computeNormals();

};
