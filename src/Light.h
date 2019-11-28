#pragma once

#include <glm/glm.hpp>

struct Light
{
	glm::vec4 m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	Light() = default;

	Light(const Light& other) = default;

	Light(Light&& other) = default;

	Light& operator=(const Light& other) = default;

	Light& operator=(Light&& other) = default;

	virtual ~Light() = default;
};
