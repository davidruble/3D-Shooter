#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Window.h"


enum CamMoveDir
{
	C_NONE,
	C_FORWARD,
	C_BACKWARD,
	C_RIGHT,
	C_LEFT
};

class Camera
{
private:
	glm::vec3 e;	//position
	glm::vec3 d;	//look at
	glm::vec3 up;	//orientation "up"
	glm::mat4 view;	//view matrix (C inverse)

	void move(CamMoveDir moveDir, glm::vec3 rightVec, float deltaTime);
	void look(glm::vec3 rightVec);

public:
	Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up);
	~Camera();

	void update(CamMoveDir moveDir, float horizAngle, float vertAngle, float deltaTime);

	glm::mat4 getCInv() { return view; }
};

#endif