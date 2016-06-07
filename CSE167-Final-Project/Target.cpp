#include "Target.h"
#include "globals.h"
#include "objects.h"
#include "straight.h"

GLuint textID;

Target::Target() {
	parse();

	colliding = false;
	alive = true;
	showBounding = true;

	toWorld = glm::mat4();

	shader = new Shader("../targetShader.vert", "../targetShader.frag");
	boxShader = new Shader("../line.vert", "../line.frag");
	
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

	glGenTextures(1, &textID);

	int width = 10;
	int height = 10;

	//glBindTexture(GL_TEXTURE_2D, textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textID);
	printf("yellow plz\n");
	unsigned char * image;
	image = loadPPM("../yellow.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	image = loadPPM("../yellow.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	image = loadPPM("../yellow.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	image = loadPPM("../yellow.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	image = loadPPM("../yellow.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	image = loadPPM("../yellow.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glBindTexture(GL_TEXTURE_2D, 0);

	wOne = new Straight();
	wTwo = new Straight();
	wThree = new Straight();
	wFour = new Straight();
	wFive = new Straight();
	wSix = new Straight();
	wSeven = new Straight();
	wEight = new Straight();
	wNine = new Straight();
	wTen = new Straight();
	wEleven = new Straight();
	wTwelve = new Straight();


}

Target::~Target() {

}

void Target::parse() {
	FILE* fp;
	int c1, c2;
	float x, x_n, y, y_n, z, z_n, r, g, b;
	xmin = NULL;
	xmax = NULL;
	ymin = NULL;
	ymax = NULL;
	zmin = NULL;
	zmax = NULL;
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


	printf("max: %f, %f, %f\n", xmax, ymax, zmax);
	printf("min: %f, %f, %f\n", xmin, ymin, zmin);

	xmax = (xmax - xmin) / scalingFactor - 1.0f;
	xmin = (xmin - xmin) / scalingFactor - 1.0f;

	ymax = (ymax - ymin) / scalingFactor - 1.0f;
	ymin = (ymin - ymin) / scalingFactor - 1.0f;

	zmax = (zmax - zmin) / scalingFactor - 1.0f;
	zmin = (zmin - zmin) / scalingFactor - 1.0f;


	printf("max: %f, %f, %f\n", xmax, ymax, zmax);
	printf("min: %f, %f, %f\n", xmin, ymin, zmin);
}



void Target::draw() {
	if (alive) {
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


		GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPosition");
		GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLuint shineDamperLoc = glGetUniformLocation(shaderProgram, "shineDamper");
		GLuint reflectivityLoc = glGetUniformLocation(shaderProgram, "reflectivity");
		glUniform3fv(lightPosLoc, 1, &Global::sun->getPosition()[0]);
		glUniform3fv(lightColorLoc, 1, &Global::sun->getColor()[0]);
		glUniform1f(shineDamperLoc, Global::t_shineDamper);
		glUniform1f(reflectivityLoc, Global::t_reflectivity);

		glUniform1i(glGetUniformLocation(shaderProgram, "smile"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textID);

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
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDisable(GL_BLEND);

		if (showBounding) {
			glm::vec3 w1 = glm::vec3(toWorld * glm::vec4(xmax, ymax, zmax, 1));
			glm::vec3 w2 = glm::vec3(toWorld * glm::vec4(xmax, ymax, zmin, 1));
			glm::vec3 w3 = glm::vec3(toWorld * glm::vec4(xmax, ymin, zmax, 1));
			glm::vec3 w4 = glm::vec3(toWorld * glm::vec4(xmax, ymin, zmin, 1));
			glm::vec3 w5 = glm::vec3(toWorld * glm::vec4(xmin, ymax, zmax, 1));
			glm::vec3 w6 = glm::vec3(toWorld * glm::vec4(xmin, ymax, zmin, 1));
			glm::vec3 w7 = glm::vec3(toWorld * glm::vec4(xmin, ymin, zmax, 1));
			glm::vec3 w8 = glm::vec3(toWorld * glm::vec4(xmin, ymin, zmin, 1));

			wOne->color = glm::vec3(1, 0, 1);
			wTwo->color = glm::vec3(1, 0, 1);
			wThree->color = glm::vec3(1, 0, 1);
			wFour->color = glm::vec3(1, 0, 1);
			wFive->color = glm::vec3(1, 0, 1);
			wSix->color = glm::vec3(1, 0, 1);
			wSeven->color = glm::vec3(1, 0, 1);
			wEight->color = glm::vec3(1, 0, 1);
			wNine->color = glm::vec3(1, 0, 1);
			wTen->color = glm::vec3(1, 0, 1);
			wEleven->color = glm::vec3(1, 0, 1);
			wTwelve->color = glm::vec3(1, 0, 1);

			if (colliding && Global::isFiring) {

				printf("hit: %f, %f, %f\n", hit[0], hit[1], hit[2]);

				//Right
				if (hit.x == xmax) {
					wOne->color = glm::vec3(0, 1, 0);
					wTwo->color = glm::vec3(0, 1, 0);
					wThree->color = glm::vec3(0, 1, 0);
					wFour->color = glm::vec3(0, 1, 0);
				}

				//Top
				if (hit.y == ymax) {
					wOne->color = glm::vec3(0, 1, 0);
					wTen->color = glm::vec3(0, 1, 0);
					wFive->color = glm::vec3(0, 1, 0);
					wNine->color = glm::vec3(0, 1, 0);
				}

				//Left
				if (hit.x == xmin) {
					wFive->color = glm::vec3(0, 1, 0);
					wSix->color = glm::vec3(0, 1, 0);
					wSeven->color = glm::vec3(0, 1, 0);
					wEight->color = glm::vec3(0, 1, 0);
				}

				//Bottom
				if (hit.y == ymin) {
					wThree->color = glm::vec3(0, 1, 0);
					wTwelve->color = glm::vec3(0, 1, 0);
					wSeven->color = glm::vec3(0, 1, 0);
					wEleven->color = glm::vec3(0, 1, 0);
				}

				//Front
				if (hit.z == zmax) {
					wEleven->color = glm::vec3(0, 1, 0);
					wFour->color = glm::vec3(0, 1, 0);
					wNine->color = glm::vec3(0, 1, 0);
					wEight->color = glm::vec3(0, 1, 0);
				}

				//Back
				if (hit.z == zmin) {
					wTen->color = glm::vec3(0, 1, 0);
					wSix->color = glm::vec3(0, 1, 0);
					wTwelve->color = glm::vec3(0, 1, 0);
					wTwo->color = glm::vec3(0, 1, 0);
				}

			}


			
			wOne->set(w1, w2);
			wTwo->set(w2, w4);
			wThree->set(w3, w4);
			wFour->set(w3, w1);
			wFive->set(w5, w6);
			wSix->set(w6, w8);
			wSeven->set(w7, w8);
			wEight->set(w7, w5);
			wNine->set(w1, w5);
			wTen->set(w2, w6);
			wEleven->set(w3, w7);
			wTwelve->set(w4, w8);

			boxShader->use();
			wOne->draw(boxShader->getProgram());
			wTwo->draw(boxShader->getProgram());
			wThree->draw(boxShader->getProgram());
			wFour->draw(boxShader->getProgram());
			wFive->draw(boxShader->getProgram());
			wSix->draw(boxShader->getProgram());
			wSeven->draw(boxShader->getProgram());
			wEight->draw(boxShader->getProgram());
			wNine->draw(boxShader->getProgram());
			wTen->draw(boxShader->getProgram());
			wEleven->draw(boxShader->getProgram());
			wTwelve->draw(boxShader->getProgram());
		}
	}
}