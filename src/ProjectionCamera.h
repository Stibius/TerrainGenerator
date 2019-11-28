#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ProjectionCamera
{

public:

	ProjectionCamera() = default;

	ProjectionCamera(const ProjectionCamera& other) = default;

	ProjectionCamera(ProjectionCamera&& other) = default;

	ProjectionCamera& operator=(const ProjectionCamera& other) = default;

	ProjectionCamera& operator=(ProjectionCamera&& other) = default;

	virtual ~ProjectionCamera() = default;

	virtual glm::mat4 getProjectionMatrix() = 0;

};
