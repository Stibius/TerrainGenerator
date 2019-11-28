#pragma once

#include "ProjectionCamera.h"

class OrthoCamera : public ProjectionCamera
{

protected:

	float m_left = -1.0f;
	float m_right = 1.0f;
	float m_bottom = -1.0f;
	float m_top = 1.0f;
	float m_near = 2.0f;
	float m_far = 10000.0f;
	glm::mat4 m_projectionMatrix;

	bool m_recompute = true;

	void computeProjection();

public:

	OrthoCamera() = default;

	OrthoCamera(float left, float right, float bottom, float top, float Near, float Far);

	OrthoCamera(const OrthoCamera& other) = default;

	OrthoCamera(OrthoCamera&& other) = default;

	OrthoCamera& operator=(const OrthoCamera& other) = default;

	OrthoCamera& operator=(OrthoCamera&& other) = default;

	~OrthoCamera() override = default;

	void setProjection(float left, float right, float bottom, float top, float Near, float Far);

	void setLeft(float left);

	void setRight(float right);

	void setBottom(float bottom);

	void setTop(float top);

	void setNear(float Near);

	void setFar(float Far);

	float getLeft() const;

	float getRight() const;

	float getBottom() const;

	float getTop() const;

	float getNear() const;

	float getFar() const;

	glm::mat4 getProjectionMatrix() override;

};
