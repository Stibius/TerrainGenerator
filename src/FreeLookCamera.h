#pragma once

#include "ViewCamera.h"

class FreeLookCamera : public ViewCamera
{

private:

	glm::vec3 m_position;
	glm::vec3 m_angles;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_rotationMatrix;
	bool m_recomputeView = true;
	bool m_recomputeRotation = true;

	void computeView();

	void computeRotation();

public:

	FreeLookCamera() = default;

	FreeLookCamera(const glm::vec3 position, const glm::vec3 viewVector, const glm::vec3 upVector);

	FreeLookCamera(const FreeLookCamera& other) = default;

	FreeLookCamera(FreeLookCamera&& other) = default;

	FreeLookCamera& operator=(const FreeLookCamera& other) = default;

	FreeLookCamera& operator=(FreeLookCamera&& other) = default;

	~FreeLookCamera() override = default;

	void up(float dy);

	void down(float dy);

	void left(float dx);

	void right(float dx);

	void forward(float dz);

	void back(float dz);

	void move(size_t axis, float distance);

	void setPosition(glm::vec3 position);

	void addPosition(glm::vec3 delta);

	void setXAngle(float value);

	void setYAngle(float value);

	void setZAngle(float value);

	void setAngle(size_t axis, float value);

	void addXAngle(float delta);

	void addYAngle(float delta);

	void addZAngle(float delta);

	void addAngle(size_t axis, float delta);

	void setRotation(glm::vec3 viewVector, glm::vec3 upVector);

	float getXAngle() const;

	float getYAngle() const;

	float getZAngle() const;

	float getAngle(size_t axis) const;

	glm::mat4 getRotationMatrix();

	glm::vec3 getPosition() override;

	glm::mat4 getViewMatrix() override;

};
