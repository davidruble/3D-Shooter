#include "skybox.h"
#include "objects.h"
#include "globals.h"

GLfloat sb_vertices[] = {
	// Front vertices
	-0.5, -0.5,  0.5,
	0.5, -0.5,  0.5,
	0.5,  0.5,  0.5,
	-0.5,  0.5,  0.5,
	// Back vertices
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5,  0.5, -0.5,
	-0.5,  0.5, -0.5
};

GLuint sb_indices[] = {
	// Front face
	0, 1, 2,
	2, 3, 0,
	// Top face
	1, 5, 6,
	6, 2, 1,
	// Back face
	7, 6, 5,
	5, 4, 7,
	// Bottom face
	4, 0, 3,
	3, 7, 4,
	// Left face
	4, 5, 1,
	1, 0, 4,
	// Right face
	3, 2, 6,
	6, 7, 3
};

Skybox::Skybox()
{
	this->toWorld = glm::mat4(1.0f);

	// create the buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//bind the VAO
	glBindVertexArray(VAO);
		//copy the vertices in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sb_vertices), sb_vertices, GL_STATIC_DRAW);

		//copy the face data into an element buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sb_indices), sb_indices, GL_STATIC_DRAW);

		//set the vertex positions attribute pointer
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind the VAO
	glBindVertexArray(0);

	//get all the faces
	//faces.push_back("skybox/hw_crater/right.ppm");
	//faces.push_back("skybox/hw_crater/left.ppm");
	/*
	faces.push_back("skybox/hw_crater/left.ppm");
	faces.push_back("skybox/hw_crater/right.ppm");
	faces.push_back("skybox/hw_crater/top.ppm");
	faces.push_back("skybox/hw_crater/bottom.ppm");
	faces.push_back("skybox/hw_crater/front.ppm");
	faces.push_back("skybox/hw_crater/back.ppm");
	*/

	faces.push_back("skybox/front.ppm");
	faces.push_back("skybox/back.ppm");
	faces.push_back("skybox/top.ppm");
	faces.push_back("skybox/bottom.ppm");
	faces.push_back("skybox/right.ppm");
	faces.push_back("skybox/left.ppm");
	
	//setup the cubemap for the skybox
	loadCubemap();

	//make it bigger!
	scale(500.0f);
}

Skybox::~Skybox()
{
	//deallocate all resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Skybox::loadCubemap()
{
	int width, height;
	unsigned char* image;

	//create ID for the texture
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	//setup the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		//load up all the faces
		for (GLuint i = 0; i < faces.size(); i++)
		{
			image = loadPPM(faces[i], width, height);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		}

		// Make sure no bytes are padded:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Select GL_MODULATE to mix texture with polygon color for shading:
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// Use bilinear interpolation:
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Use clamp to edge to hide skybox edges:
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::draw(GLuint shaderProgram)
{
	glm::mat4 view = glm::mat4(glm::mat3(Global::camera->getCInv()));

	//supply the model, view, and projection matrices to the shader
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]); 
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Window::P[0][0]);

	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);

	//draw the skybox cube
	glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
}

void Skybox::scale(float amount) 
{
	//scale the object by the specified amount
	toWorld = glm::scale(toWorld, glm::vec3(amount, amount, amount));
}
