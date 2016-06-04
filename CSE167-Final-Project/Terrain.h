#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "globals.h"

class Terrain
{
private:
	GLuint VAO;
	GLuint verts_VBO;
	GLuint norms_VBO;
	GLuint textCoords_VBO;
	GLuint inds_EBO;

	GLuint shaderProgram;

	float* vertices;
	float* normals;
	float* textureCoords;
	int* indices;

	glm::mat4 toWorld;

	void generateTerrain();

public:
	Terrain();
	~Terrain();

	void render(GLuint textureID);
};

#endif