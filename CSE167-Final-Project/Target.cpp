#include "Target.h"
#include "globals.h"
#include "objects.h"

Target::Target() {
	parse();

	colliding = false;
	alive = true;
	showBounding = false;

	toWorld = glm::mat4();

	shader = new Shader("../targetShader.vert", "../targetShader.frag");
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), &(vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), &(indices[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		sizeof(glm::vec3), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

Target::~Target() {

}

void Target::parse() {
	FILE* fp;
	int c1, c2;
	float x, x_n, y, y_n, z, z_n, r, g, b;
	float xmin = NULL;
	float xmax = NULL;
	float ymin = NULL;
	float ymax = NULL;
	float zmin = NULL;
	float zmax = NULL;
	int f_1, f_2, f_3;
	fp = fopen("../sphere.obj", "r");
	c1 = fgetc(fp);
	while (c1 != EOF) {
		if (c1 == 'v') {
			c2 = fgetc(fp);
			if (c2 == ' ') {
				fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				vertices.push_back(glm::vec3(x, y, z));
				if (x > xmax || xmax == NULL) xmax = x;
				if (x < xmin || xmin == NULL) xmin = x;
				if (y > ymax || ymax == NULL) ymax = y;
				if (y < ymin || ymin == NULL) ymin = y;
				if (z > zmax || zmax == NULL) zmax = z;
				if (z < zmin || zmin == NULL) zmin = z;

			}
			else if (c2 == 'n') {
				fscanf(fp, "%f %f %f", &x_n, &y_n, &z_n);
				normals.push_back(glm::normalize(glm::vec3(x_n, y_n, z_n)));
			}
		}
		else if (c1 == 'f') {
			c2 = fgetc(fp);
			if (c2 == ' ') {
				fscanf(fp, "%d//%d %d//%d %d//%d", &f_1, &f_1, &f_2, &f_2, &f_3, &f_3);
				indices.push_back(f_1 - 1);
				indices.push_back(f_2 - 1);
				indices.push_back(f_3 - 1);
			}
		}
		c1 = fgetc(fp);

	}
	fclose(fp);

	float scalingFactor;
	if (xmax - xmin > ymax - ymin && xmax - xmin > zmax - zmin) scalingFactor = (xmax - xmin) / 2.0f;
	else if (ymax - ymin > xmax - xmin && ymax - ymin > zmax - zmin) scalingFactor = (ymax - ymin) / 2.0f;
	else scalingFactor = (zmax - zmin) / 2.0f;

	for (int i = 0; i < vertices.size(); i++) {
		glm::vec3 updatedVec = vertices[i];
		updatedVec[0] = (updatedVec[0] - xmin) / scalingFactor - 1.0f;
		updatedVec[1] = (updatedVec[1] - ymin) / scalingFactor - 1.0f;
		updatedVec[2] = (updatedVec[2] - zmin) / scalingFactor - 1.0f;
		vertices[i] = updatedVec;
	}

	xmin = (xmin - xmin) / scalingFactor - 1.0f;
	xmax = (xmax - xmin) / scalingFactor - 1.0f;
	ymin = (ymin - ymin) / scalingFactor - 1.0f;
	ymax = (ymax - ymin) / scalingFactor - 1.0f;
	zmin = (zmin - zmin) / scalingFactor - 1.0f;
	zmax = (zmax - zmin) / scalingFactor - 1.0f;
}

void Target::draw() {
	shader->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLuint shaderProgram = shader->getProgram();
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &Global::camera->getCInv()[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Window::P[0][0]);

	GLuint camLoc = glGetUniformLocation(shaderProgram, "cameraPos");
	glUniform3fv(camLoc, 1, &Global::camera->getPos()[0]);

	GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPosition");
	GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLuint shineDamperLoc = glGetUniformLocation(shaderProgram, "shineDamper");
	GLuint reflectivityLoc = glGetUniformLocation(shaderProgram, "reflectivity");
	glUniform3fv(lightPosLoc, 1, &Global::sun->getPosition()[0]);
	glUniform3fv(lightColorLoc, 1, &Global::sun->getColor()[0]);
	glUniform1f(shineDamperLoc, Global::t_shineDamper);
	glUniform1f(reflectivityLoc, Global::t_reflectivity);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}