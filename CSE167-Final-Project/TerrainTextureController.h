#ifndef TERRAIN_TEXTURE_CONTROLLER_H
#define TERRAIN_TEXTURE_CONTROLLER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "globals.h"
#include "Texture.h"

enum TerrainTextureType
{
	GRASS,
	DESERT,
	SNOW
};

class TerrainTextureController
{
private:
	Texture * lowTexture;
	Texture * midTexture;
	Texture * highTexture;

	void initTextures(TerrainTextureType type, bool initializing);

public:
	TerrainTextureController(TerrainTextureType initType);
	~TerrainTextureController();

	void setTextures(TerrainTextureType type);

	GLuint getLowID() { return lowTexture->getTexture(); }
	GLuint getMidID() { return midTexture->getTexture(); }
	GLuint getHighID() { return highTexture->getTexture(); }
};

#endif
