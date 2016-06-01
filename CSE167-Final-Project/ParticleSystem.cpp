#include "ParticleSystem.h"
#include "objects.h"

const GLfloat particle_vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f
};

GLfloat* ParticleSystem::particle_position_size_data = new GLfloat[Global::MAX_PARTICLES * 4];
GLubyte* ParticleSystem::particle_color_data = new GLubyte[Global::MAX_PARTICLES * 4];

ParticleSystem::ParticleSystem()
{
	this->shaderProgram = Global::particleShader->getProgram();
	this->numParticles = 0;
	this->lastUsedParticle = 0;

	//initialize the particle values
	for (int i = 0; i < Global::MAX_PARTICLES; ++i)
	{
		particlesContainer[i].life = -1.0f;
		particlesContainer[i].cameraDistance = -1.0f;
	}

	//create the vertex buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &billboard_VBO);
	glGenBuffers(1, &particlesPosition_VBO);
	glGenBuffers(1, &particlesColor_VBO);

	//bind the vertex array object
	glBindVertexArray(VAO);
		//copy the vertices into a vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, billboard_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);

		//set up the VBO containing the positions and sizes of the particles
		//initialize with empty buffer that will be updated later
		glBindBuffer(GL_ARRAY_BUFFER, particlesPosition_VBO);
		glBufferData(GL_ARRAY_BUFFER, Global::MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

		//set up the VBO containing the colors of the particles
		//initialize with empty buffer that will be updated later
		glBindBuffer(GL_ARRAY_BUFFER, particlesColor_VBO);
		glBufferData(GL_ARRAY_BUFFER, Global::MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBindVertexArray(0);

	lastTime = glfwGetTime();
}

ParticleSystem::~ParticleSystem()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &billboard_VBO);
	glDeleteBuffers(1, &particlesPosition_VBO);
	glDeleteBuffers(1, &particlesColor_VBO);
	delete[] particle_position_size_data;
	delete[] particle_color_data;
}

void ParticleSystem::render(glm::vec3 startPos, glm::vec3 direction)
{
	double currentTime = glfwGetTime();
	delta = currentTime - lastTime;
	lastTime = currentTime;

	generateParticles(startPos, direction);
	simulateParticles();

	//supply the camera right and up vectors to the shader
	GLuint cameraRightLoc = glGetUniformLocation(shaderProgram, "cameraRight");
	GLuint cameraUpLoc = glGetUniformLocation(shaderProgram, "cameraUp");
	glUniform3fv(cameraRightLoc, 1, &Global::camera->getRight()[0]);
	glUniform3fv(cameraUpLoc, 1, &Global::camera->getUp()[0]);

	//supply the ViewProjection matrix to the shader
	glm::mat4 VP = Window::P * Global::camera->getCInv();
	GLuint viewProjLoc = glGetUniformLocation(shaderProgram, "ViewProj");
	glUniformMatrix4fv(viewProjLoc, 1, GL_FALSE, &VP[0][0]);

	glBindVertexArray(VAO);
		//update the particle positions buffer
		glBindBuffer(GL_ARRAY_BUFFER, particlesPosition_VBO);
		glBufferData(GL_ARRAY_BUFFER, Global::MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * sizeof(GLfloat) * 4, particle_position_size_data);

		//update the particle color buffer
		glBindBuffer(GL_ARRAY_BUFFER, particlesColor_VBO);
		glBufferData(GL_ARRAY_BUFFER, Global::MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * sizeof(GLubyte) * 4, particle_color_data);

		//set the vertex attribute pointers with the new data
		//vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_VBO);
		glVertexAttribPointer(
			0, //attribute number in the shader
			3, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized?
			0, //stride
			(void*)0 //offset
		);

		//positions of particles' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particlesPosition_VBO);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particlesColor_VBO);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

		//set the parameters for instanced rendering
		glVertexAttribDivisor(0, 0); //always reuse same 4 vertices
		glVertexAttribDivisor(1, 1); //one position per quad (it's center)
		glVertexAttribDivisor(2, 1); //one color per quad

		//draw the particles
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numParticles);

		//unbind the vertex attribute arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

//find a particle in the particleContainer which isn't used yet
//used in creating new particles
int ParticleSystem::findUnusedParticle()
{
	//search after the last used particle
	for (int i = lastUsedParticle; i < Global::MAX_PARTICLES; ++i)
	{
		//found an unused particle
		if (particlesContainer[i].life < 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	//search before the last used particle
	for (int i = 0; i < lastUsedParticle; ++i)
	{
		//found an unused particle
		if (particlesContainer[i].life < 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	//all particles are taken, so override the first one
	lastUsedParticle = 0;
	return 0;
}

void ParticleSystem::generateParticles(glm::vec3 startPos, glm::vec3 direction)
{
	//generate new particles each second, limiting it to 60fps
	int newParticles = (int)(delta * Global::PARTICLES_PER_SECOND);
	if (newParticles > (int)(0.016f * Global::PARTICLES_PER_SECOND))
		newParticles = (int)(0.016f * Global::PARTICLES_PER_SECOND);

	//generate each new particle
	for (int i = 0; i < newParticles; ++i)
	{
		int pIndex = findUnusedParticle();
		particlesContainer[pIndex].life = Global::PARTICLE_LIFE;
		particlesContainer[pIndex].pos = startPos;

		//randomize the particle direction a little bit
		glm::vec3 randomDir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);
		particlesContainer[pIndex].speed = direction + randomDir*Global::PARTICLE_SPREAD;
		//glm::vec3 partSpeed = particlesContainer[pIndex].speed;
		//cerr << "speed: " << "(" << partSpeed.x << ", " << partSpeed.y << ", " << partSpeed.z << ")" << std::endl;
		
		//generate a random red color for the particle
		particlesContainer[pIndex].r = rand() % 256;
		particlesContainer[pIndex].g = 0;
		particlesContainer[pIndex].b = 0;
		particlesContainer[pIndex].a = (rand() % 256) / 3;

		//set the size
		particlesContainer[pIndex].size = Global::PARTICLE_SIZE;
	}
}

void ParticleSystem::simulateParticles()
{
	//simulate all the particles
	numParticles = 0;
	lastUsedParticle = 0;
	for (int i = 0; i < Global::MAX_PARTICLES; ++i)
	{
		Particle& p = particlesContainer[i];

		//only work with particles that are still alive
		if (p.life > 0.0f)
		{
			//decrease the life of the particle and check if it's still alive
			p.life -= delta;
			if (p.life > 0.0f)
			{
				//move the particle forward
				if (p.pos.z >= 0)
					p.pos += (p.speed * (float)delta);
				if (p.pos.z < 0)
					p.pos -= (p.speed * (float)delta);
				p.cameraDistance = glm::abs(glm::length(p.pos - Global::camera->getPos()));

				//fill the position buffer
				particle_position_size_data[4 * numParticles + 0] = p.pos.x;
				particle_position_size_data[4 * numParticles + 1] = p.pos.y;
				particle_position_size_data[4 * numParticles + 2] = p.pos.z;
				particle_position_size_data[4 * numParticles + 3] = p.size;

				//fill the color buffer
				particle_color_data[4 * numParticles + 0] = p.r;
				particle_color_data[4 * numParticles + 1] = p.g;
				particle_color_data[4 * numParticles + 2] = p.b;
				particle_color_data[4 * numParticles + 3] = p.a;
			}

			//particle just died
			else
			{
				//sort to the end of the buffer
				p.cameraDistance = -1.0f;
			}

			++numParticles;
		}
	}
	sortParticles();
}

//sort the objects back to front
void ParticleSystem::sortParticles()
{
	std::sort(&particlesContainer[0], &particlesContainer[Global::MAX_PARTICLES]);
}

void ParticleSystem::killParticles()
{
	//reset all the particles
	numParticles = 0;
	for (int i = 0; i < Global::MAX_PARTICLES; ++i)
	{
		particlesContainer[i].life = -1.0f;
		particlesContainer[i].cameraDistance = -1.0f;
	}
}