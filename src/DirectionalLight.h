#pragma once

#include "Light.h"

struct DirectionalLight : public Light
{
	glm::vec4 m_direction = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
};
