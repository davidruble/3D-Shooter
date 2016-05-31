#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "globals.h"

struct Particle
{
	glm::vec3 pos, speed;
	unsigned char r, g, b, a;
	float size, angle, weight;
	float life;
	float cameraDistance;

	//used to sort particles -- farther particles drawn first
	bool operator<(const Particle& other) const
	{
		return this->cameraDistance > other.cameraDistance;
	}
};

class ParticleSystem
{
private:
	GLuint VAO;
	GLuint billboard_VBO;
	GLuint particlesPosition_VBO;
	GLuint particlesColor_VBO;

	GLuint shaderProgram;

	Particle particlesContainer[Global::MAX_PARTICLES];
	static GLfloat* particle_position_size_data;
	static GLubyte* particle_color_data;

	int numParticles;
	int lastUsedParticle;

	double lastTime;
	double delta;

	int findUnusedParticle();
	void generateParticles(glm::vec3 startPos, glm::vec3 direction);
	void simulateParticles();
	void sortParticles();

public:
	ParticleSystem();
	~ParticleSystem();

	void render(glm::vec3 startPos, glm::vec3 direction);
};

#endif
