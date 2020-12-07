#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(float x, float y, float z);
	glm::mat4 getView();
	glm::vec3 getPosition() const;
	void rotateCamera(float radius);

private:
	glm::vec3 position;
	float angle;
};

