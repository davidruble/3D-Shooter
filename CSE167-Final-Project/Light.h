#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

//implemented as a point light
class Light
{
private:
	glm::vec3 position;
	glm::vec3 color;

public:
	Light(glm::vec3 pos, glm::vec3 color) : position(pos), color(color) {}
	~Light() {}

	glm::vec3 getPosition() { return position; }
	glm::vec3 getColor() { return color; }
};

#endif