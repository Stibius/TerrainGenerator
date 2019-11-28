#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera 
{

public:

	Camera() = default;

	Camera(const Camera& other) = default;

	Camera(Camera&& other) = default;

	Camera& operator=(const Camera& other) = default;

	Camera& operator=(Camera&& other) = default;

	virtual ~Camera() = default;

	virtual glm::vec3 getPosition() = 0;

	virtual glm::mat4 getViewMatrix() = 0;

	virtual glm::mat4 getProjectionMatrix() = 0;

	virtual glm::mat4 getViewProjectionMatrix() = 0;

};
