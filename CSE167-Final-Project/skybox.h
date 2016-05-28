#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
using std::vector;

#include "ppmLoader.h"
#include "Window.h"

class Skybox
{
private:
	GLuint VAO, VBO, EBO;
	GLuint textureID;
	vector<const GLchar*> faces;
	glm::mat4 toWorld;

	void loadCubemap();

public:
	Skybox();
	~Skybox();
	void draw(GLuint shaderProgram);
	void scale(float amount);

	GLuint getTextureID() { return textureID; }
};

#endif