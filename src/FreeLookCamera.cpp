
#define GLM_ENABLE_EXPERIMENTAL

#include<glm/gtx/euler_angles.hpp>
#include<glm/gtc/matrix_access.hpp>

#include "FreeLookCamera.h"

void FreeLookCamera::computeView()
{
	if (m_recomputeRotation == true)
	{
		computeRotation();
	}

	m_viewMatrix = m_rotationMatrix * glm::translate(glm::mat4(1.0f), -m_position);

	m_recomputeView = false;
}

void FreeLookCamera::computeRotation()
{
	m_rotationMatrix = glm::eulerAngleXYZ(m_angles.x, m_angles.y, m_angles.z);

	m_recomputeRotation = false;
}

FreeLookCamera::FreeLookCamera(glm::vec3 position, glm::vec3 viewVector, glm::vec3 upVector)
{
	setPosition(position);
	setRotation(viewVector, upVector);
}

void FreeLookCamera::up(float dy)
{
	move(1, dy);
}

void FreeLookCamera::down(float dy)
{
	move(1, -dy);
}

void FreeLookCamera::left(float dx)
{
	move(0, -dx);
}

void FreeLookCamera::right(float dx)
{
	move(0, dx);
}

void FreeLookCamera::forward(float dz)
{
	move(2, -dz);
}

void FreeLookCamera::back(float dz)
{
	move(2, dz);
}

void FreeLookCamera::move(size_t axis, float distance)
{
	if (axis >= 3) return;

	m_position += distance * glm::vec3(glm::row(m_rotationMatrix, static_cast<glm::length_t>(axis)));

	m_recomputeView = true;
}

void FreeLookCamera::setPosition(glm::vec3 position)
{
	m_position = position;

	m_recomputeView = true;
}

void FreeLookCamera::addPosition(glm::vec3 delta)
{
	m_position += delta;

	m_recomputeView = true;
}

void FreeLookCamera::setXAngle(float value)
{
	setAngle(0, value);
}

void FreeLookCamera::setYAngle(float value)
{
	setAngle(1, value);
}

void FreeLookCamera::setZAngle(float value)
{
	setAngle(2, value);
}

void FreeLookCamera::setAngle(size_t axis, float value)
{
	if (axis >= 3) return;

	if (axis == 0)
	{
		m_angles[static_cast<uint32_t>(axis)] = glm::clamp(value, -glm::half_pi<float>(), glm::half_pi<float>());
	}
	else
	{
		m_angles[static_cast<uint32_t>(axis)] = value;
	}

	m_recomputeRotation = true;
	m_recomputeView = true;
}

void FreeLookCamera::addXAngle(float delta)
{
	setAngle(0, getAngle(0) + delta);
}

void FreeLookCamera::addYAngle(float delta)
{
	setAngle(1, getAngle(1) + delta);
}

void FreeLookCamera::addZAngle(float delta)
{
	setAngle(2, getAngle(2) + delta);
}

void FreeLookCamera::addAngle(size_t axis, float delta)
{
	if (axis >= 3) return;

	setAngle(axis, getAngle(axis) + delta);
}

void FreeLookCamera::setRotation(glm::vec3 viewVector, glm::vec3 upVector)
{
	glm::vec3 xx = glm::normalize(glm::cross(viewVector, upVector));
	glm::vec3 yy = glm::normalize(glm::cross(xx, viewVector));
	glm::vec3 zz = glm::normalize(glm::cross(xx, yy));
	m_rotationMatrix[0] = glm::vec4(xx, 0.0f);
	m_rotationMatrix[1] = glm::vec4(yy, 0.0f);
	m_rotationMatrix[2] = glm::vec4(zz, 0.0f);
	m_rotationMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_rotationMatrix = glm::transpose(m_rotationMatrix);
	m_recomputeRotation = false;
	m_recomputeView = true;
}

glm::vec3 FreeLookCamera::getPosition() 
{
	return m_position;
}

float FreeLookCamera::getXAngle() const
{
	return getAngle(0);
}

float FreeLookCamera::getYAngle() const
{
	return getAngle(1);
}

float FreeLookCamera::getZAngle() const
{
	return getAngle(2);
}

float FreeLookCamera::getAngle(size_t axis) const
{
	if (axis >= 3) return 0.0f;

	return m_angles[static_cast<uint32_t>(axis)];
}

glm::mat4 FreeLookCamera::getRotationMatrix()
{
	if (m_recomputeRotation == true)
	{
		computeRotation();
	}

	return m_rotationMatrix;
}

glm::mat4 FreeLookCamera::getViewMatrix()
{
	if (m_recomputeView == true)
	{
		computeView();
	}

	return m_viewMatrix;
}
