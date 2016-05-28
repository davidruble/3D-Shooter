#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Window.h"

enum CamTransform
{
	C_NONE,
	C_MOVE,
	C_LOOK
};

class Camera
{
private:
	glm::vec3 e;	//position
	glm::vec3 d;	//look at
	glm::vec3 up;	//orientation "up"
	glm::mat4 view;	//view matrix (C inverse)

public:
	Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up);
	~Camera();

	void update(CamTransform transop, float x, float y, float z);
	void move(float x, float y, float z);
	void look_at(); //TODO

	glm::mat4 getCInv() { return view; }
};

#endif