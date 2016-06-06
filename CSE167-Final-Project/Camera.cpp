#include "Camera.h"
#include "globals.h"
#include "objects.h"

#include <iostream>

Camera::Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up)
{
	//set all the member variables
	this->e = pos;
	this->d = lookAt;
	this->up = up;	
	this->view = glm::lookAt(e, d, up);
	this->right = glm::cross(up, d);
}

Camera::~Camera()
{

}

void Camera::update(CamMoveDir moveDir, float &horizAngle, float &vertAngle, float deltaTime)
{
	//check for terrain collision
	/*float terrainHeight = Global::terrain->getHeightOfTerrain(this->e.x, this->e.z);
	if (this->e.y <= terrainHeight)
	{
		//don't go any lower than the terrain
		this->e.y = terrainHeight + Global::CAM_Y_OFFSET;
	}
	else
	{
		//fall with gravity
		this->e.y += Global::GRAVITY * deltaTime;
	}*/

	//clamp the vertical looking angle
	if (vertAngle > Global::VERTICAL_CLAMP)
		vertAngle = Global::VERTICAL_CLAMP;
	else if (vertAngle < -Global::VERTICAL_CLAMP)
		vertAngle = -Global::VERTICAL_CLAMP;

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
	move(moveDir, deltaTime);
	lookAround();
}

void Camera::move(CamMoveDir moveDir, float deltaTime)
{
	switch (moveDir)
	{
	//move forward
	case C_FORWARD:
		this->e.x += glm::normalize(this->d).x * deltaTime * Global::MOVE_SPEED;
		this->e.y += glm::normalize(this->d).y * deltaTime * Global::MOVE_SPEED;
		this->e.z += glm::normalize(this->d).z * deltaTime * Global::MOVE_SPEED;
		break;

	//move backwards
	case C_BACKWARD:
		this->e.x -= glm::normalize(this->d).x * deltaTime * Global::MOVE_SPEED;
		this->e.y -= glm::normalize(this->d).y * deltaTime * Global::MOVE_SPEED;
		this->e.z -= glm::normalize(this->d).z * deltaTime * Global::MOVE_SPEED;
		break;

	//strafe right
	case C_RIGHT:
		this->e.x += glm::normalize(this->right).x * deltaTime * Global::MOVE_SPEED;
		this->e.y += glm::normalize(this->right).y * deltaTime * Global::MOVE_SPEED;
		this->e.z += glm::normalize(this->right).z * deltaTime * Global::MOVE_SPEED;
		break;

	//strafe left
	case C_LEFT:
		this->e.x -= glm::normalize(this->right).x * deltaTime * Global::MOVE_SPEED;
		this->e.y -= glm::normalize(this->right).y * deltaTime * Global::MOVE_SPEED;
		this->e.z -= glm::normalize(this->right).z * deltaTime * Global::MOVE_SPEED;
		break;

	default:
		return;
	}

	//restrict the y position of the free roaming camera
	if (e.y > Global::T_MAX_HEIGHT - Global::CAM_Y_OFFSET)
		e.y = Global::T_MAX_HEIGHT - Global::CAM_Y_OFFSET;
	else if (e.y < Global::CAM_LOWER_BOUND + Global::CAM_Y_OFFSET)
		e.y = Global::CAM_LOWER_BOUND + Global::CAM_Y_OFFSET;

	//update the camera's view
	this->view = glm::lookAt(e, e + d, up);
}

void Camera::lookAround()
{
	//calculate the new up vector
	this->up = glm::cross(right, d);

	//update the camera's view
	this->view = glm::lookAt(e, e + d, up);
}
