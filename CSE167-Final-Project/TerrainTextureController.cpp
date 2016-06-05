#include "TerrainTextureController.h"

TerrainTextureController::TerrainTextureController(TerrainTextureType initType)
{
	initTextures(initType, true);
}

TerrainTextureController::~TerrainTextureController()
{
	delete(lowTexture);
	delete(midTexture);
	delete(highTexture);
}

void TerrainTextureController::setTextures(TerrainTextureType type)
{
	initTextures(type, false);
}

void TerrainTextureController::initTextures(TerrainTextureType type, bool initializing)
{
	//initialize the texture
	switch (type)
	{
	case GRASS:
		if (initializing)
		{
			lowTexture = new Texture(Global::GRASS_LOW_IMG);
			midTexture = new Texture(Global::GRASS_MID_IMG);
			highTexture = new Texture(Global::GRASS_HIGH_IMG);
		}
		else
		{
			lowTexture->setTexture(Global::GRASS_LOW_IMG);
			midTexture->setTexture(Global::GRASS_MID_IMG);
			highTexture->setTexture(Global::GRASS_HIGH_IMG);
		}
		break;

	case DESERT:
		if (initializing)
		{
			lowTexture = new Texture(Global::DESERT_LOW_IMG);
			midTexture = new Texture(Global::DESERT_MID_IMG);
			highTexture = new Texture(Global::DESERT_MID_IMG);
		}
		else
		{
			lowTexture->setTexture(Global::DESERT_LOW_IMG);
			midTexture->setTexture(Global::DESERT_MID_IMG);
			highTexture->setTexture(Global::DESERT_MID_IMG);
		}
		break;

	case SNOW:
		if (initializing)
		{
			lowTexture = new Texture(Global::SNOW_LOW_IMG);
			midTexture = new Texture(Global::SNOW_MID_IMG);
			highTexture = new Texture(Global::SNOW_HIGH_IMG);
		}
		else
		{
			lowTexture->setTexture(Global::SNOW_LOW_IMG);
			midTexture->setTexture(Global::SNOW_MID_IMG);
			highTexture->setTexture(Global::SNOW_HIGH_IMG);
		}
		break;

	default:
		std::cerr << "Unknown texture type!" << std::endl;
		break;
	}
}
