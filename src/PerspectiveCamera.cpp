
#include "PerspectiveCamera.h"

void PerspectiveCamera::computeProjection()
{
	m_projectionMatrix = glm::perspective(m_fovy, m_aspect, m_near, m_far);

	m_recompute = false;
}

PerspectiveCamera::PerspectiveCamera(float aspect, float fovy, float Near, float Far)
{
	setProjection(aspect, fovy, Near, Far);
}

void PerspectiveCamera::setProjection(float aspect, float fovy, float Near, float Far)
{
	m_aspect = aspect;
	m_fovy = fovy;
	m_near = Near;
	m_far = Far;

	m_recompute = true;
}

void PerspectiveCamera::setFovy(float fovy)
{
	m_fovy = fovy;

	m_recompute = true;
}

void PerspectiveCamera::setNear(float Near)
{
	m_near = Near;

	m_recompute = true;
}

void PerspectiveCamera::setFar(float Far)
{
	m_far = Far;

	m_recompute = true;
}

void PerspectiveCamera::setAspect(float aspect)
{
	m_aspect = aspect;

	m_recompute = true;
}

float PerspectiveCamera::getFovy() const
{
	return m_fovy;
}

float PerspectiveCamera::getNear() const
{
	return m_near;
}

float PerspectiveCamera::getFar() const
{
	return m_far;
}

float PerspectiveCamera::getAspect() const
{
	return m_aspect;
}

glm::mat4 PerspectiveCamera::getProjectionMatrix() 
{
	if (m_recompute == true)
	{
		computeProjection();
	}

	return m_projectionMatrix;
}
