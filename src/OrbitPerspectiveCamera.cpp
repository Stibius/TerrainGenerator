
#include "OrbitPerspectiveCamera.h"

OrbitPerspectiveCamera::OrbitPerspectiveCamera(const PerspectiveCamera& perspectiveCamera, const OrbitCamera& orbitCamera)
	: m_perspectiveCamera(perspectiveCamera)
	, m_orbitCamera(orbitCamera)
{

}

glm::vec3 OrbitPerspectiveCamera::getPosition()
{
	return m_orbitCamera.getPosition();
}

glm::mat4 OrbitPerspectiveCamera::getViewMatrix()
{
	return m_orbitCamera.getViewMatrix();
}

glm::mat4 OrbitPerspectiveCamera::getProjectionMatrix()
{
	return m_perspectiveCamera.getProjectionMatrix();
}

glm::mat4 OrbitPerspectiveCamera::getViewProjectionMatrix()
{
	return m_perspectiveCamera.getProjectionMatrix() * m_orbitCamera.getViewMatrix();
}