#ifndef OBJECTS_H
#define OBJECTS_H

#include "skybox.h"
#include "Camera.h"
#include "shader.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "Texture.h"
#include "Light.h"

namespace Global
{
	extern Skybox * skybox;
	extern Camera * camera;
	extern Shader * skyboxShader;
	extern Shader * particleShader;
	extern Shader * terrainShader;
	extern ParticleSystem * particleSystem;
	extern Terrain * terrain;
	extern Texture * texture;
	extern Light * sun;
}

#endif