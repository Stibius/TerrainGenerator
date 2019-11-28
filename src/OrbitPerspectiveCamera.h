#pragma once

#include "OrbitCamera.h"
#include "PerspectiveCamera.h"
#include "Camera.h"

class OrbitPerspectiveCamera : public Camera
{

public:

	PerspectiveCamera m_perspectiveCamera;
	OrbitCamera m_orbitCamera;

	OrbitPerspectiveCamera() = default;

	OrbitPerspectiveCamera(const PerspectiveCamera& perspectiveCamera, const OrbitCamera& orbitCamera);

	OrbitPerspectiveCamera(const OrbitPerspectiveCamera& other) = default;

	OrbitPerspectiveCamera(OrbitPerspectiveCamera&& other) = default;

	OrbitPerspectiveCamera& operator=(const OrbitPerspectiveCamera& other) = default;

	OrbitPerspectiveCamera& operator=(OrbitPerspectiveCamera&& other) = default;

	~OrbitPerspectiveCamera() override = default;

	glm::vec3 getPosition() override;

	glm::mat4 getViewMatrix() override;

	glm::mat4 getProjectionMatrix() override;

	glm::mat4 getViewProjectionMatrix() override;

};
