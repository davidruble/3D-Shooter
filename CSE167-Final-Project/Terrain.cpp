#include "Terrain.h"
#include "objects.h"

using Global::T_VERTEX_COUNT;
using Global::T_SIZE;

Terrain::Terrain()
{
	this->shaderProgram = Global::terrainShader->getProgram();
	this->toWorld = glm::scale(Global::terrainScale);
	this->toWorld = glm::translate(glm::mat4(1.0f), Global::terrainOffset);

	//initialize the data buffers
	int count = T_VERTEX_COUNT * T_VERTEX_COUNT;
	this->vertices = new float[Global::T_IND_VERT_SIZE];
	this->normals = new float[Global::T_IND_VERT_SIZE];
	this->textureCoords = new float[Global::T_TEX_COORD_SIZE];
	this->indices = new int[Global::T_INDS_SIZE];

	generateTerrain();

	//create the vertex buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &inds_EBO);
	glGenBuffers(1, &verts_VBO);
	glGenBuffers(1, &textCoords_VBO);
	glGenBuffers(1, &norms_VBO);

	//bind the vertex array object
	glBindVertexArray(VAO);
		
		//bind the indices buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inds_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * Global::T_INDS_SIZE, indices, GL_STATIC_DRAW);

		//bind the vertices buffer
		glBindBuffer(GL_ARRAY_BUFFER, verts_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * Global::T_IND_VERT_SIZE, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//bind the texture coords buffer
		glBindBuffer(GL_ARRAY_BUFFER, textCoords_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords) * Global::T_TEX_COORD_SIZE, textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//bind the normals buffer
		glBindBuffer(GL_ARRAY_BUFFER, norms_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * Global::T_IND_VERT_SIZE, normals, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &inds_EBO);
	glDeleteBuffers(1, &verts_VBO);
	glDeleteBuffers(1, &textCoords_VBO);
	glDeleteBuffers(1, &norms_VBO);
	delete[] indices;
	delete[] vertices;
	delete[] textureCoords;
	delete[] normals;
}

void Terrain::generateTerrain()
{
	//initialize the vertices, normals, and texture coords
	int v = 0;
	for (int i = 0; i < T_VERTEX_COUNT; ++i)
	{
		for (int j = 0; j < T_VERTEX_COUNT; ++j)
		{
			vertices[v * 3 + 0] = (float)j / ((float)T_VERTEX_COUNT - 1) * T_SIZE;
			vertices[v * 3 + 1] = 0.0f;
			vertices[v * 3 + 2] = (float)i / ((float)T_VERTEX_COUNT - 1) * T_SIZE;
			normals[v * 3 + 0] = 0.0f;
			normals[v * 3 + 1] = 1.0f;
			normals[v * 3 + 2] = 0.0f;
			textureCoords[v * 2 + 0] = (float)j / ((float)T_VERTEX_COUNT - 1);
			textureCoords[v * 2 + 1] = (float)i / ((float)T_VERTEX_COUNT - 1);
			++v;
		}
	}

	//initialize the indices
	int ind = 0;
	for (int gz = 0; gz < T_VERTEX_COUNT - 1; ++gz)
	{
		for (int gx = 0; gx < T_VERTEX_COUNT - 1; ++gx)
		{
			int topLeft = (gz * T_VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * T_VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			indices[ind++] = topLeft;
			indices[ind++] = bottomLeft;
			indices[ind++] = topRight;
			indices[ind++] = topRight;
			indices[ind++] = bottomLeft;
			indices[ind++] = bottomRight;
		}
	}
}

void Terrain::render(GLuint textureID)
{
	//supply the model, view, and projection matrices to the shader
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &Global::camera->getCInv()[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Window::P[0][0]);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(shaderProgram, "modelTexture"), 0);

	//draw the terrain
	glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, Global::T_INDS_SIZE, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}