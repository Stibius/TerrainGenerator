#pragma once

#include <glm/glm.hpp>

class Mesh;

class AABB
{

public:

	glm::vec3 m_min = glm::vec3(0.0f);
	glm::vec3 m_max = glm::vec3(0.0f);
	glm::vec3 m_size = glm::vec3(0.0f);

	AABB() = default;

	explicit AABB(const Mesh& mesh);

	AABB(const AABB& other) = default;

	AABB(AABB&& other) = default;

	AABB& operator=(const AABB& other) = default;

	AABB& operator=(AABB&& other) = default;

	~AABB() = default;

	void compute(const Mesh& mesh);

	float getLongestSide() const;

};
