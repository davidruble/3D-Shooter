#include "Camera.h"
#include "globals.h"

#include <iostream>

Camera::Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up)
{
	//set all the member variables
	this->e = pos;
	this->d = lookAt;
	this->up = up;	
	this->view = glm::lookAt(e, d, up);
	this->right = glm::cross(d, up);
}

Camera::~Camera()
{

}

void Camera::update(CamMoveDir moveDir, float horizAngle, float vertAngle, float deltaTime)
{
	//calculate the new direction
	this->d = glm::vec3(
		glm::cos(vertAngle) * glm::sin(horizAngle),
		glm::sin(vertAngle),
		glm::cos(vertAngle) * glm::cos(horizAngle)
	);

	//calculate right vector
	this->right = glm::vec3(
		glm::sin(horizAngle - (glm::pi<float>() / 2.0f)),
		0.0f,
		glm::cos(horizAngle - (glm::pi<float>() / 2.0f))
	);

	//update the camera
	look(right);
	move(moveDir, right, deltaTime);
}

void Camera::move(CamMoveDir moveDir, glm::vec3 rightVec, float deltaTime)
{
	switch (moveDir)
	{
	//move forward
	case C_FORWARD:
		this->e.x += glm::normalize(this->d).x * deltaTime * Global::MOVE_SPEED;
		this->e.z += glm::normalize(this->d).z * deltaTime * Global::MOVE_SPEED;
		break;

	//move backwards
	case C_BACKWARD:
		this->e.x -= glm::normalize(this->d).x * deltaTime * Global::MOVE_SPEED;
		this->e.z -= glm::normalize(this->d).z * deltaTime * Global::MOVE_SPEED;
		break;

	//strafe right
	case C_RIGHT:
		this->e.x += glm::normalize(rightVec).x * deltaTime * Global::MOVE_SPEED;
		this->e.z += glm::normalize(rightVec).z * deltaTime * Global::MOVE_SPEED;
		break;

	//strafe left
	case C_LEFT:
		this->e.x -= glm::normalize(rightVec).x * deltaTime * Global::MOVE_SPEED;
		this->e.z -= glm::normalize(rightVec).z * deltaTime * Global::MOVE_SPEED;
		break;

	default:
		return;
	}
	
	//update the camera's view
	this->view = glm::lookAt(e, e + d, up);
}

void Camera::look(glm::vec3 rightVec)
{
	//calculate the new up vector
	this->up = glm::cross(rightVec, d);

	//update the camera's view
	this->view = glm::lookAt(e, e + d, up);
}
