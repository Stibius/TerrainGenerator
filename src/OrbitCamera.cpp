
#include "OrbitCamera.h"

void OrbitCamera::computeView()
{
	glm::mat4 rotationMatrix =
		glm::rotate(glm::mat4(1.0f), m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

	m_viewMatrix =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -m_distance)) *
		rotationMatrix *
		glm::translate(glm::mat4(1.0f), m_target);

	m_position = glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	m_recomputeView = false;
}

OrbitCamera::OrbitCamera(glm::vec3 target, glm::vec2 rotation, float distance)
{
	setView(target, rotation, distance);
}

void OrbitCamera::setView(glm::vec3 target, glm::vec2 rotation, float distance)
{
	m_target = target;
	m_rotation = rotation;
	m_distance = distance;

	m_recomputeView = true;
}

void OrbitCamera::setTargetPosition(glm::vec3 target)
{
	m_target = target;

	m_recomputeView = true;
}

void OrbitCamera::addTargetPosition(glm::vec3 target)
{
	m_target += target;

	m_recomputeView = true;
}

void OrbitCamera::setRotation(glm::vec2 rotation)
{
	m_rotation = rotation;

	m_recomputeView = true;
}

void OrbitCamera::addRotation(glm::vec2 rotation)
{
	m_rotation += rotation;

	m_recomputeView = true;
}

void OrbitCamera::setDistance(float distance)
{
	m_distance = distance;

	m_recomputeView = true;
}

void OrbitCamera::addDistance(float distance)
{
	m_distance += distance;

	m_recomputeView = true;
}

glm::vec3 OrbitCamera::getTarget() const
{
	return m_target;
}

glm::vec2 OrbitCamera::getRotation() const
{
	return m_rotation;
}

float OrbitCamera::getDistance() const
{
	return m_distance;
}

glm::vec3 OrbitCamera::getPosition()
{
	if (m_recomputeView == true)
	{
		computeView();
	}

	return m_position;
}

glm::mat4 OrbitCamera::getViewMatrix() 
{
	if (m_recomputeView == true)
	{
		computeView();
	}

	return m_viewMatrix;
}
