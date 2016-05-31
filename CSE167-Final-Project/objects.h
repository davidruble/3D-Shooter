#ifndef OBJECTS_H
#define OBJECTS_H

#include "skybox.h"
#include "Camera.h"
#include "shader.h"
#include "ParticleSystem.h"

namespace Global
{
	extern Skybox * skybox;
	extern Camera * camera;
	extern Shader * skyboxShader;
	extern Shader * particleShader;
	extern ParticleSystem * particleSystem;
}

#endif