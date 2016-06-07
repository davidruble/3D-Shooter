#include "Straight.h"
#include "Window.h"
#include "globals.h"
#include "objects.h"

Straight::Straight() {
	one = glm::vec3(1.0f, 0.0f, 1.0f);
	two = glm::vec3(1.0f, 0.0f, 1.0f);
	//calculate();
	setofPoints.push_back(one);
	setofPoints.push_back(two);
	color = glm::vec3(0.9920f, 0.369f, 0.325f);


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*setofPoints.size(), &(setofPoints[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		sizeof(glm::vec3), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Straight::~Straight() {

}


void Straight::draw(GLuint shaderProgram) {

	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &Global::camera->getCInv()[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Window::P[0][0]);
	GLuint ColorID = glGetUniformLocation(shaderProgram, "colors");
	glUniform3fv(ColorID, 1, &color[0]);
	glLineWidth(3.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	glBindVertexArray(0);

}

void Straight::update() {

}

void Straight::calculate() {
	setofPoints.clear();
	for (float t = 0.0f; t < 1; t = t + 0.49) {
		float Czero = 1.0f - t;
		float Cone = t;

		glm::vec3 Presult = Czero * one + Cone * two;
		setofPoints.push_back(glm::vec3());
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*setofPoints.size(), &(setofPoints[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		sizeof(glm::vec3), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glLineWidth(1.0f);


}

void Straight::set(glm::vec3 u, glm::vec3 d) {
	one = u;
	two = d;

	setofPoints.clear();
	setofPoints.push_back(one);
	setofPoints.push_back(two);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*setofPoints.size(), &(setofPoints[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		sizeof(glm::vec3), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glLineWidth(1.0f);
}
