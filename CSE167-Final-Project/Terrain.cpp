#include "Terrain.h"
#include "objects.h"

using Global::T_VERTEX_COUNT;
using Global::T_SIZE;

int T_IND_VERT_SIZE;
int T_TEX_COORD_SIZE;
int T_INDS_SIZE;

Terrain::Terrain(bool useHeightmap, const char* heightmapImage)
{
	this->shaderProgram = Global::terrainShader->getProgram();
	this->toWorld = glm::scale(Global::terrainScale);
	this->toWorld = glm::translate(glm::mat4(1.0f), Global::terrainOffset);

	//load the heightmap (if applicable)
	if (useHeightmap)
	{
		heightmap = loadPPM(heightmapImage, heightmapWidth, heightmapHeight);
		vertexCount = heightmapHeight;

		//if image not loaded correctly, default to flat
		if (heightmap == NULL)
		{
			std::cerr << "Defaulting to flat terrain" << std::endl;
			vertexCount = Global::T_VERTEX_COUNT;
			useHeightmap = false;
		}
	}
	else
	{
		vertexCount = Global::T_VERTEX_COUNT;
	}

	//set the sizes of the buffers based on the vertex count
	T_IND_VERT_SIZE = vertexCount * vertexCount * 3;
	T_TEX_COORD_SIZE = vertexCount * vertexCount * 2;
	T_INDS_SIZE = 6 * (vertexCount - 1) * (vertexCount - 1);

	//initialize the data buffers
	int count = vertexCount * vertexCount;
	this->vertices = new float[T_IND_VERT_SIZE];
	this->normals = new float[T_IND_VERT_SIZE];
	this->textureCoords = new float[T_TEX_COORD_SIZE];
	this->indices = new int[T_INDS_SIZE];

	generateTerrain(useHeightmap);

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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * T_INDS_SIZE, indices, GL_STATIC_DRAW);

		//bind the vertices buffer
		glBindBuffer(GL_ARRAY_BUFFER, verts_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * T_IND_VERT_SIZE, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//bind the texture coords buffer
		glBindBuffer(GL_ARRAY_BUFFER, textCoords_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords) * T_TEX_COORD_SIZE, textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//bind the normals buffer
		glBindBuffer(GL_ARRAY_BUFFER, norms_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * T_IND_VERT_SIZE, normals, GL_STATIC_DRAW);
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

void Terrain::generateTerrain(bool useHeightmap)
{
	//initialize the vertices, normals, and texture coords
	int v = 0;
	for (int i = 0; i < vertexCount; ++i)
	{
		for (int j = 0; j < vertexCount; ++j)
		{
			glm::vec3 normal = calculateNormal(j, i);

			vertices[v * 3 + 0] = (float)j / ((float)vertexCount - 1) * T_SIZE;
			vertices[v * 3 + 1] = useHeightmap ? getImageHeight(j, i) : 0.0f;
			vertices[v * 3 + 2] = (float)i / ((float)vertexCount - 1) * T_SIZE;
			normals[v * 3 + 0] = normal.x;
			normals[v * 3 + 1] = normal.y;
			normals[v * 3 + 2] = normal.z;
			textureCoords[v * 2 + 0] = (float)j / ((float)vertexCount - 1);
			textureCoords[v * 2 + 1] = (float)i / ((float)vertexCount - 1);
			++v;
		}
	}

	//initialize the indices
	int ind = 0;
	for (int gz = 0; gz < vertexCount - 1; ++gz)
	{
		for (int gx = 0; gx < vertexCount - 1; ++gx)
		{
			int topLeft = (gz * vertexCount) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * vertexCount) + gx;
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

//calculates the new normal for the new height of the terrain at IMAGE position x,z
glm::vec3 Terrain::calculateNormal(int x, int z)
{
	float heightL = getImageHeight(x - 1, z);
	float heightR = getImageHeight(x + 1, z);
	float heightD = getImageHeight(x, z - 1);
	float heightU = getImageHeight(x, z + 1);

	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
	normal = glm::normalize(normal);
	return normal;
}

//gets the height of the terrain at the x,z IMAGE coordinate
float Terrain::getImageHeight(int x, int z)
{
	//make sure the values are within the buffer bounds
	if (x < 0 || z < 0 || x > heightmapHeight * heightmapWidth || z > heightmapHeight * heightmapWidth)
		return 0.0f;

	float height = (float)getRGB(x, z);
	height += Global::T_MAX_PIXEL_COLOR / 2.0f;
	height /= Global::T_MAX_PIXEL_COLOR / 2.0f;
	height *= Global::T_MAX_HEIGHT;
	return height;
}

//gets the RGB value as a single int at coordinate (x, z) of the IMAGE
int Terrain::getRGB(int x, int z)
{
	if (heightmap == NULL)
		return 0;
	else
	{
		int index = z * heightmapWidth + x;
		int r = heightmap[3 * index + 0];
		int g = heightmap[3 * index + 1];
		int b = heightmap[3 * index + 2];

		int rgb = r;
		rgb = (rgb << 8) + g;
		rgb = (rgb << 8) + b;
		return rgb;
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

	//supply the shader with light properties
	GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPosition");
	GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLuint shineDamperLoc = glGetUniformLocation(shaderProgram, "shineDamper");
	GLuint reflectivityLoc = glGetUniformLocation(shaderProgram, "reflectivity");
	glUniform3fv(lightPosLoc, 1, &Global::sun->getPosition()[0]);
	glUniform3fv(lightColorLoc, 1, &Global::sun->getColor()[0]);
	glUniform1f(shineDamperLoc, Global::t_shineDamper);
	glUniform1f(reflectivityLoc, Global::t_reflectivity);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(shaderProgram, "modelTexture"), 0);

	//draw the terrain
	glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, T_INDS_SIZE, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}