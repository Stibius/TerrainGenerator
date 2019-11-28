
#include "FreeLookOrthoCamera.h"

FreeLookOrthoCamera::FreeLookOrthoCamera(const OrthoCamera& orthoCamera, const FreeLookCamera& freeLookCamera)
	: m_orthoCamera(orthoCamera)
	, m_freeLookCamera(freeLookCamera)
{

}

glm::vec3 FreeLookOrthoCamera::getPosition()
{
	return m_freeLookCamera.getPosition();
}

glm::mat4 FreeLookOrthoCamera::getViewMatrix()
{
	return m_freeLookCamera.getViewMatrix();
}

glm::mat4 FreeLookOrthoCamera::getProjectionMatrix()
{
	return m_orthoCamera.getProjectionMatrix();
}

glm::mat4 FreeLookOrthoCamera::getViewProjectionMatrix()
{
	return m_orthoCamera.getProjectionMatrix() * m_freeLookCamera.getViewMatrix();
}