#include "Camera.h"

Camera::Camera(float x, float y, float z) 
	: position(x,y,z), angle(0), angleIncrement(0)
{
}

glm::mat4 Camera::getView()
{
	return glm::lookAt(
		position,
		glm::vec3(0, 0, 0), 
		glm::vec3(0, 1, 0)
	);
}

void Camera::rotateCamera(float radius) {
	angle += angleIncrement;

	if (angle > glm::pi<float>() * 2)
		angle -= glm::pi<float>() * 2;

	position.x = radius * sin(angle);
	position.y = 2;
	position.z = radius * cos(angle);
}

glm::vec3 Camera::getPosition() const{
	return position;
}

void Camera::setAngleIncrement(float a) {
	angleIncrement = a;
}