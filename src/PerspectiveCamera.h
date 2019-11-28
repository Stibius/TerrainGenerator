#pragma once

#include "ProjectionCamera.h"

class PerspectiveCamera : public ProjectionCamera
{

protected:

	float m_fovy = glm::radians(45.0f);
	float m_near = 2.0f;
	float m_far = 10000.0f;
	float m_aspect = 1024.0f / 768.0f;
	glm::mat4 m_projectionMatrix;

	bool m_recompute = true;

	void computeProjection();

public:

	PerspectiveCamera() = default;

	PerspectiveCamera(float aspect, float fovy, float Near, float Far);

	PerspectiveCamera(const PerspectiveCamera& other) = default;

	PerspectiveCamera(PerspectiveCamera&& other) = default;

	PerspectiveCamera& operator=(const PerspectiveCamera& other) = default;

	PerspectiveCamera& operator=(PerspectiveCamera&& other) = default;

	~PerspectiveCamera() override = default;

	void setProjection(float aspect, float fovy, float Near, float Far);

	void setFovy(float fovy);

	void setNear(float Near);

	void setFar(float Far);

	void setAspect(float aspect);

	float getFovy() const;

	float getNear() const;

	float getFar() const;

	float getAspect() const;

	glm::mat4 getProjectionMatrix() override;

};
