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
}

Camera::~Camera()
{

}

void Camera::update(CamTransform transop, float x, float y, float z)
{
	//TODO
	switch (transop)
	{
	//move the camera in xyz
	case C_MOVE:
		break;

	//looking around with the mouse
	case C_LOOK:
		break;
	}
}

void Camera::move(float x, float y, float z)
{

}

void Camera::look_at()
{

}
