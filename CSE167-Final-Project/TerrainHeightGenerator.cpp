#include "TerrainHeightGenerator.h"

TerrainHeightGenerator::TerrainHeightGenerator()
{
	//set the seed
	reseed();
}

//set the height to a random value between +/- amplitude
float TerrainHeightGenerator::generateHeight(int x, int z)
{
	float total = 0.0f;
	float d = glm::pow(2, Global::OCTAVES - 1);
	for (int i = 0; i < Global::OCTAVES; ++i)
	{
		float freq = (float) glm::pow(2, i) / d;
		float amp = (float) glm::pow(Global::TERRAIN_ROUGHNESS, i) * Global::HEIGHT_AMPLITUDE;
		total += getInterpolatedNoise(x*freq / 2.0f, z*freq / 2.0f) * amp;
	}
	return total;
}

//generate a height based on the interpolated height of the nearest points
float TerrainHeightGenerator::getInterpolatedNoise(float x, float z)
{
	float v1 = getSmoothNoise(x, z);
	float v2 = getSmoothNoise(x + 1, z);
	float v3 = getSmoothNoise(x, z + 1);
	float v4 = getSmoothNoise(x + 1, z + 1);

	float i1 = interpolate(v1, v2, (float)(x - (long)x));
	float i2 = interpolate(v3, v4, (float)(x - (long)x));
	
	return interpolate(i1, i2, (float)(z -(long)z));
}

//get the smoothed average for the nearby vertices
float TerrainHeightGenerator::getSmoothNoise(int x, int z)
{
	float corners = (getNoise(x - 1, z - 1) + getNoise(x + 1, z - 1) + 
		getNoise(x - 1, z + 1) + getNoise(x + 1, z + 1)) / 16.0f;
	float sides = (getNoise(x - 1, z) + getNoise(x + 1, z) + 
		getNoise(x, z - 1) + getNoise(x, z + 1)) / 8.0f;
	float center = getNoise(x, z) / 4.0f;

	return corners + sides + center;
}

//generate height noise
float TerrainHeightGenerator::getNoise(int x, int z)
{
	srand(x * 49632 + z * 325176 + seed);
	return ((double)rand() / (double)RAND_MAX) * 2.0f - 0.2f;
}

//use cosine interpolation to interpolate between two points
float TerrainHeightGenerator::interpolate(float a, float b, float blend)
{
	double theta = blend * glm::pi<float>();
	float f = (1.0f - glm::cos(theta)) * 0.5f;
	return a * (1.0f - f) + b * f;
}

void TerrainHeightGenerator::reseed()
{
	srand(time(NULL) * 123456);
	seed = rand() % 1000000000;
}