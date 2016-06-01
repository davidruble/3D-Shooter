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
	const float MOVE_SPEED = 5.0f;
	const float LOOK_SPEED = 0.003f;

	//properties for objects in scene -- chrome
	const glm::vec3 objAmbient = glm::vec3(0.25f, 0.25f, 0.25f);
	const glm::vec3 objDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	const glm::vec3 objSpecular = glm::vec3(0.774597f, 0.774597f, 0.774597f);
	const GLfloat objShininess = 0.5f * 128.0f;

	//particle properties
	const int MAX_PARTICLES = 1000;
	const int PARTICLES_PER_SECOND = 2000;
	const float PARTICLE_LIFE = 2.0f;
	const float PARTICLE_SPREAD = 5.0f;
	const float PARTICLE_SIZE = 0.02f;
	const glm::vec3 PARTICLE_SPEED = glm::vec3(10.0f, 10.0f, 10.0f);
	const glm::vec3 camOffset = glm::vec3(0.0f, -0.25f, 0.0f);
	extern bool isFiring;
};

#endif
