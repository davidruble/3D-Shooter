#ifndef _STRAIGHT_H_
#define _STRAIGHT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <vector>
#include <math.h>

class Straight {

public:
	Straight();
	~Straight();

	glm::vec3 one;
	glm::vec3 two;
	std::vector<glm::vec3> setofPoints;

	GLuint VBO, VAO, EBO, NBO;
	glm::vec3 color;

	void draw(GLuint);
	void update();
	void calculate();
	void set(glm::vec3, glm::vec3);

};

#endif