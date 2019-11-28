
#include "OrthoCamera.h"

void OrthoCamera::computeProjection()
{
	m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);

	m_recompute = false;
}

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float Near, float Far)
{
	setProjection(left, right, bottom, top, Near, Far);
}

void OrthoCamera::setProjection(float left, float right, float bottom, float top, float Near, float Far)
{
	m_left = left;
	m_right = right;
	m_bottom = bottom;
	m_top = top;
	m_near = Near;
	m_far = Far;

	m_recompute = true;
}

void OrthoCamera::setLeft(float left)
{
	m_left = left;

	m_recompute = true;
}

void OrthoCamera::setRight(float right)
{
	m_right = right;

	m_recompute = true;
}

void OrthoCamera::setBottom(float bottom)
{
	m_bottom = bottom;

	m_recompute = true;
}

void OrthoCamera::setTop(float top)
{
	m_top = top;

	m_recompute = true;
}

void OrthoCamera::setNear(float Near)
{
	m_near = Near;

	m_recompute = true;
}

void OrthoCamera::setFar(float Far)
{
	m_far = Far;

	m_recompute = true;
}

float OrthoCamera::getLeft() const
{
	return m_left;
}

float OrthoCamera::getRight() const
{
	return m_right;
}

float OrthoCamera::getBottom() const
{
	return m_bottom;
}

float OrthoCamera::getTop() const
{
	return m_top;
}

float OrthoCamera::getNear() const
{
	return m_near;
}

float OrthoCamera::getFar() const
{
	return m_far;
}

glm::mat4 OrthoCamera::getProjectionMatrix()
{
	if (m_recompute == true)
	{
		computeProjection();
	}

	return m_projectionMatrix;
}
