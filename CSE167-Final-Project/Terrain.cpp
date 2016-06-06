#include "Terrain.h"
#include "objects.h"
#include "globals.h"

using Global::T_VERTEX_COUNT;
using Global::T_SIZE;
using Global::terrainTextureController;
using Global::terrainHeightGenerator;

int T_IND_VERT_SIZE;
int T_TEX_COORD_SIZE;
int T_INDS_SIZE;

bool Global::useHeightmap = true;

TerrainHeightGenerator * Global::terrainHeightGenerator;
TerrainTextureController * Global::terrainTextureController;

Terrain::Terrain(const char* heightmapImage)
{
	//initialize the texture controller with the grass texture set
	terrainTextureController = new TerrainTextureController(GRASS);
	terrainHeightGenerator = new TerrainHeightGenerator();

	this->shaderProgram = Global::terrainShader->getProgram();
	this->toWorld = glm::scale(Global::terrainScale);
	this->toWorld = glm::translate(glm::mat4(1.0f), Global::terrainOffset);
	this->heights = vector<float>();

	//load the heightmap
	heightmap = loadPPM(heightmapImage, heightmapWidth, heightmapHeight);

	//if image not loaded correctly, default to flat
	if (heightmap == NULL)
	{
		std::cerr << "Defaulting to flat terrain" << std::endl;
		vertexCount = Global::T_VERTEX_COUNT;
		Global::useHeightmap = false;
	}

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
	delete terrainTextureController;
	delete terrainHeightGenerator;
}

void Terrain::generateTerrain()
{
	if (Global::useHeightmap)
	{
		vertexCount = heightmapHeight;
	}
	else
	{
		vertexCount = T_VERTEX_COUNT;
		Global::terrainHeightGenerator->reseed();
	}

	//set the sizes of the buffers based on the vertex count
	T_IND_VERT_SIZE = vertexCount * vertexCount * 3;
	T_TEX_COORD_SIZE = vertexCount * vertexCount * 2;
	T_INDS_SIZE = 6 * (vertexCount - 1) * (vertexCount - 1);

	//initialize the data buffers
	delete[] indices;
	delete[] vertices;
	delete[] textureCoords;
	delete[] normals;
	int count = vertexCount * vertexCount;
	this->vertices = new float[T_IND_VERT_SIZE];
	this->normals = new float[T_IND_VERT_SIZE];
	this->textureCoords = new float[T_TEX_COORD_SIZE];
	this->indices = new int[T_INDS_SIZE];
	this->heights.resize(vertexCount * vertexCount);

	//initialize the vertices, normals, and texture coords
	int v = 0;
	for (int i = 0; i < vertexCount; ++i)
	{
		for (int j = 0; j < vertexCount; ++j)
		{
			glm::vec3 normal = calculateNormal(j, i);
			float height = getHeight(j, i);
			heights[(j * vertexCount) + i] = height;

			vertices[v * 3 + 0] = (float)j / ((float)vertexCount - 1) * T_SIZE;
			vertices[v * 3 + 1] = height;
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

float Terrain::getHeightOfTerrain(float worldX, float worldZ)
{
	float terrainX = Global::T_TRANS_VAL - worldX;
	float terrainZ = Global::T_TRANS_VAL - worldZ;
	//float terrainX = worldX + Global::T_TRANS_VAL;
	//float terrainZ = worldZ + Global::T_TRANS_VAL;

	//calculate the grid properties at worldX, worldY coord
	float gridSquareSize = Global::T_SIZE / ((float)heights.size() - 1.0f);
	//float gridSquareSize = vertexCount * vertexCount / ((float)heights.size() - 1.0f);
	int gridZ = -glm::floor(terrainX * gridSquareSize);
	int gridX = -glm::floor(terrainZ * gridSquareSize);

	//std::cerr << "World: " << worldX << ", " << worldZ << std::endl;
	//std::cerr << "Terrain: " << terrainX << ", " << terrainZ << std::endl;
	//std::cerr << "Grid: " << gridX << ", " << gridZ << std::endl;
	//std::cerr << "Camera: " << Global::camera->getPos().y << std::endl;
	//std::cerr << "Grid size: " << gridSquareSize << std::endl;

	if (gridX >= heights.size() - 2 || gridZ >= heights.size() - 2 || gridX - 1 < 0 || gridZ - 1 < 0)
	{
		//std::cerr << "Out of bounds!" << std::endl;
		return 0.0f;
	}

	//get the x and z coordinate of this grid
	float xCoord = fmod(terrainX, 1.0f - gridSquareSize) * gridSquareSize;
	float zCoord = fmod(terrainZ, 1.0f - gridSquareSize) * gridSquareSize;

	float answer;
	
	//player is in top triangle of this grid
	if (xCoord <= (1.0f - zCoord))
	{
		answer = baryCentric(glm::vec3(0.0f, heights[(gridX * vertexCount) + gridZ], 0.0f), 
			glm::vec3(1.0f, heights[(gridX + 1) * vertexCount + gridZ], 0.0f), 
			glm::vec3(0.0f, heights[gridX * vertexCount + (gridZ + 1)], 1.0f),
			glm::vec2(xCoord, zCoord));
	}

	//player is in the bottom triangle of this grid
	else
	{
		answer = baryCentric(glm::vec3(1.0f, heights[(gridX + 1) * vertexCount + gridZ], 0.0f), 
			glm::vec3(1.0f, heights[(gridX + 1) * vertexCount + (gridZ + 1)], 1.0f), 
			glm::vec3(0.0f, heights[gridX * vertexCount + (gridZ + 1)], 1.0f),
			glm::vec2(xCoord, zCoord));
	}
	return answer - (Global::T_MAX_HEIGHT + Global::T_SCALE_VAL);
}

//calculates the new normal for the new height of the terrain at IMAGE position x,z
glm::vec3 Terrain::calculateNormal(int x, int z)
{
	float heightL = getHeight(x - 1, z);
	float heightR = getHeight(x + 1, z);
	float heightD = getHeight(x, z - 1);
	float heightU = getHeight(x, z + 1);

	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
	normal = glm::normalize(normal);
	return normal;
}

//gets the height of the terrain at the x,z IMAGE coordinate
float Terrain::getHeight(int x, int z)
{
	if (Global::useHeightmap)
	{
		//get the height from the heightmap
		if (x < 0 || z < 0 || x > heightmapHeight * heightmapWidth || z > heightmapHeight * heightmapWidth)
			return 0.0f;

		float height = (float)getRGB(x, z);
		height += Global::T_MAX_PIXEL_COLOR / 2.0f;
		height /= Global::T_MAX_PIXEL_COLOR / 2.0f;
		height *= Global::T_MAX_HEIGHT;
		return height;
	}

	//otherwise, generate a random height
	else
	{
		return Global::terrainHeightGenerator->generateHeight(x, z);
	}

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

//find the height of the point pos in the triangle defined by p1, p2, p3
float Terrain::baryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

void Terrain::render()
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

	//bind the textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Global::terrainTextureController->getLowID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Global::terrainTextureController->getMidID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Global::terrainTextureController->getHighID());

	//pass the terrain textures to the shader
	glUniform1i(glGetUniformLocation(shaderProgram, "lowTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "midTexture"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "highTexture"), 2);

	//draw the terrain
	glBindVertexArray(VAO);
		//rebind the indices buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inds_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * T_INDS_SIZE, indices, GL_STATIC_DRAW);

		//rebind the vertices buffer
		glBindBuffer(GL_ARRAY_BUFFER, verts_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * T_IND_VERT_SIZE, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//rebind the texture coords buffer
		glBindBuffer(GL_ARRAY_BUFFER, textCoords_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords) * T_TEX_COORD_SIZE, textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//rebind the normals buffer
		glBindBuffer(GL_ARRAY_BUFFER, norms_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * T_IND_VERT_SIZE, normals, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawElements(GL_TRIANGLES, T_INDS_SIZE, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}