#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "globals.h"
#include "ppmLoader.h"

using std::vector;

class Terrain
{
private:
	GLuint VAO;
	GLuint verts_VBO;
	GLuint norms_VBO;
	GLuint textCoords_VBO;
	GLuint inds_EBO;

	GLuint shaderProgram;

	int vertexCount;
	float* vertices;
	float* normals;
	float* textureCoords;
	int* indices;
	vector<float> heights;

	glm::mat4 toWorld;

	int heightmapWidth;
	int heightmapHeight;
	unsigned char* heightmap;

	glm::vec3 calculateNormal(int x, int z);
	float getHeight(int x, int z);
	int getRGB(int x, int z);
	float baryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);

public:
	Terrain(const char* heightmapImage);
	~Terrain();

	void render();
	void generateTerrain();
	float getHeightOfTerrain(float worldX, float worldZ);
};


#endif