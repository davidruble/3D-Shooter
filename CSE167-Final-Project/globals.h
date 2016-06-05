#ifndef GLOBALS_H
#define GLOBALS_H

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Global
{
	const float ROT_SCALE = 50.0f;
	const float WIDEN_SCALE = 0.1f;
	const float XY_TRANS_SCALE = 12.0f;

	const char* const WINDOW_TITLE = "Final Project";
	const int WINDOW_WIDTH_INIT = 640;
	const int WINDOW_HEIGHT_INIT = 480;

	//initial camera properties
	const float EYE_Z_DIST = 30.0f;
	const glm::vec3 cam_pos_init(0.0f, 0.0f, EYE_Z_DIST);// e  | Position of camera
	const glm::vec3 cam_look_at_init(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
	const glm::vec3 cam_up_init(0.0f, 1.0f, 0.0f);		// up | What orientation "up" is

	//camera movement properties
	const float MOVE_SPEED = 4.0f;
	const float LOOK_SPEED = 0.003f;
	const float VERTICAL_CLAMP = 0.9f;

	//properties for objects in scene -- chrome
	const glm::vec3 objAmbient = glm::vec3(0.25f, 0.25f, 0.25f);
	const glm::vec3 objDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	const glm::vec3 objSpecular = glm::vec3(0.774597f, 0.774597f, 0.774597f);
	const GLfloat objShininess = 0.5f * 128.0f;

	//particle properties
	const int MAX_PARTICLES = 2000;
	const int PARTICLES_PER_SECOND = 1000;
	const float PARTICLE_LIFE = 2.0f;
	const float PARTICLE_SPREAD = 7.0f;
	const float PARTICLE_SIZE = 0.02f;
	const glm::vec3 PARTICLE_SPEED = glm::vec3(12.0f, 12.0f, 12.0f);
	const glm::vec3 camOffset = glm::vec3(0.0f, -0.25f, 0.0f);
	extern bool isFiring;

	//light properties
	const glm::vec3 lightPosition = glm::vec3(-100000.0f, 200000.0f, 0.0f);
	const glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	//terrain stuff
	const float T_SIZE = 250.0f;
	const float T_MAX_HEIGHT = 40.0f;
	const float T_MAX_PIXEL_COLOR = 256.0f * 256.0f * 256.0f;
	const int T_VERTEX_COUNT = 128;
	const float T_SCALE_VAL = 20.0f;
	const glm::vec3 terrainScale = glm::vec3(T_SCALE_VAL, T_SCALE_VAL, T_SCALE_VAL);
	const glm::vec3 terrainOffset = glm::vec3(-(glm::sqrt(T_SIZE) * T_SCALE_VAL - glm::sqrt(T_SIZE)) / 2.0f, -(T_MAX_HEIGHT + T_SCALE_VAL), -(glm::sqrt(T_SIZE) * T_SCALE_VAL - glm::sqrt(T_SIZE)) / 2.0f);
	const float t_shineDamper = 1.0f;
	const float t_reflectivity = 0.0f;

	//terrain texture stuff
	const char* const GRASS_LOW_IMG = "res/grass_low.png";
	const char* const GRASS_MID_IMG = "res/grass_med.png";
	const char* const GRASS_HIGH_IMG = "res/grass_high.png";
	//TODO: other textures

	//terrain height generator stuff
	extern bool useHeightmap;
	const float HEIGHT_AMPLITUDE = 50.0f;
	const int OCTAVES = 3;
	const float TERRAIN_ROUGHNESS = 0.3f;
};

#endif
