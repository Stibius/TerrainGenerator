#pragma once

#include "ViewCamera.h"

class OrbitCamera : public ViewCamera
{

protected:

	glm::vec3 m_position;
	glm::vec3 m_target = glm::vec3(0.0f);
	glm::vec2 m_rotation = glm::vec2(0.0f, 0.0f); 
	float m_distance = 1000.0f;
	glm::mat4 m_viewMatrix;
	bool m_recomputeView = true;

	void computeView();

public:

	OrbitCamera() = default;

	OrbitCamera(glm::vec3 target, glm::vec2 rotation, float distance);

	OrbitCamera(const OrbitCamera& other) = default;

	OrbitCamera(OrbitCamera&& other) = default;

	OrbitCamera& operator=(const OrbitCamera& other) = default;

	OrbitCamera& operator=(OrbitCamera&& other) = default;

	~OrbitCamera() override = default;

	void setView(glm::vec3 target, glm::vec2 rotation, float distance);

	void setTargetPosition(glm::vec3 target);

	void addTargetPosition(glm::vec3 target);

	void setRotation(glm::vec2 rotation);

	void addRotation(glm::vec2 rotation);

	void setDistance(float distance);

	void addDistance(float distance);

	glm::vec3 getTarget() const;

	glm::vec2 getRotation() const;

	float getDistance() const;

	glm::vec3 getPosition() override;

	glm::mat4 getViewMatrix() override;

};
