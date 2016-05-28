#ifndef OBJECTS_H
#define OBJECTS_H

#include "skybox.h"
#include "Camera.h"
#include "shader.h"

namespace Global
{
	extern Skybox * skybox;
	extern Camera * camera;
	extern Shader * skyboxShader;
}

#endif