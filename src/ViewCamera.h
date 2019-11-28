#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ViewCamera
{

public:

	ViewCamera() = default;

	ViewCamera(const ViewCamera& other) = default;

	ViewCamera(ViewCamera&& other) = default;

	ViewCamera& operator=(const ViewCamera& other) = default;

	ViewCamera& operator=(ViewCamera&& other) = default;

	virtual ~ViewCamera() = default;

	virtual glm::vec3 getPosition() = 0;

	virtual glm::mat4 getViewMatrix() = 0;

};
