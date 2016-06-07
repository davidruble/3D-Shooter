#ifndef _TARGET_H_
#define _TARGET_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <math.h>
#include "Window.h"
#include "shader.h"
#include "globals.h"
#include "straight.h"

class Target {
public:
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	GLuint VBO, VAO, EBO, NBO;

	glm::vec3 hit;

	Straight* wOne;
	Straight* wTwo;
	Straight* wThree;
	Straight* wFour;
	Straight* wFive;
	Straight* wSix;
	Straight* wSeven;
	Straight* wEight;
	Straight* wNine;
	Straight* wTen;
	Straight* wEleven;
	Straight* wTwelve;

	Target();
	~Target();

	float xmin;
	float xmax;
	float ymin;
	float ymax;
	float zmin;
	float zmax;

	glm::mat4 toWorld;

	bool colliding;
	bool alive;
	bool showBounding;

	Shader* shader;
	Shader* boxShader;

	void draw();
	void parse();
};

#endif