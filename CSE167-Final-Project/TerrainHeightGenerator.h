#ifndef TERRAIN_HEIGHT_GENERATOR_H
#define TERRAIN_HEIGHT_GENERATOR_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "globals.h"

//utilizes Perlin noise to generate random heights for the terrain
class TerrainHeightGenerator
{
private:
	unsigned int seed;
	float getNoise(int x, int z);
	float getSmoothNoise(int x, int z);
	float getInterpolatedNoise(float x, float z);
	float interpolate(float a, float b, float blend);

public:
	TerrainHeightGenerator();
	~TerrainHeightGenerator() {}

	float generateHeight(int x, int z);
	void reseed();
};

#endif