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
		//TODO
		break;

	case SNOW:
		//TODO
		break;

	default:
		std::cerr << "Unknown texture type!" << std::endl;
		break;
	}
}
