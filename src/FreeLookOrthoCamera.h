#pragma once

#include "Camera.h"
#include "FreeLookCamera.h"
#include "OrthoCamera.h"

class FreeLookOrthoCamera : public Camera
{

public:

	OrthoCamera m_orthoCamera;
	FreeLookCamera m_freeLookCamera;

	FreeLookOrthoCamera() = default;

	FreeLookOrthoCamera(const OrthoCamera& orthoCamera, const FreeLookCamera& freeLookCamera);

	FreeLookOrthoCamera(const FreeLookOrthoCamera& other) = default;

	FreeLookOrthoCamera(FreeLookOrthoCamera&& other) = default;

	FreeLookOrthoCamera& operator=(const FreeLookOrthoCamera& other) = default;

	FreeLookOrthoCamera& operator=(FreeLookOrthoCamera&& other) = default;

	~FreeLookOrthoCamera() override = default;

	glm::vec3 getPosition() override;

	glm::mat4 getViewMatrix() override;

	glm::mat4 getProjectionMatrix() override;

	glm::mat4 getViewProjectionMatrix() override;

};
