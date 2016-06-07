#include "window.h"
#include "globals.h"
#include "objects.h"
#include "Target.h"
#include "Cube.h"

using Global::skybox;
using Global::camera;
using Global::skyboxShader;
using Global::particleSystem;
using Global::particleShader;
using Global::terrain;
using Global::terrainShader;
using Global::sun;

//window datafields
int Window::width;
int Window::height;
glm::mat4 Window::P;

//objects in the scene
Skybox * Global::skybox;
Camera * Global::camera;
Shader * Global::skyboxShader;
ParticleSystem * Global::particleSystem;
Shader * Global::particleShader;
Terrain * Global::terrain;
Shader * Global::terrainShader;
Light * Global::sun;
CamMoveDir c_moveDir;

//camera moving parameters
double mouseX = 0.0, mouseY = 0.0;
float horizAngle = 3.14f, vertAngle = 0.0f;
float deltaTime = 0.0f;
double lastTime = 0.0;
double currentTime = 0.0;

//keep rendering more particles when true, let old ones die out when false
bool Global::isFiring = false;

//Show wireframes on collision objects;
int showWires;
int demoMode;
bool spin;

vector<Target*> collidingObjects;

glm::vec3 Global::skyColor = glm::vec3(0.5f, 0.5f, 0.5f);

void Window::initialize_objects()
{
	//initialize the particle system and shader
	particleShader = new Shader("../particle.vert", "../particle.frag");
	particleSystem = new ParticleSystem();

	// load the skybox and shader
	skyboxShader = new Shader("../skybox.vert", "../skybox.frag");
	skybox = new Skybox();

	//load the terrain and shader, and set the terrain to initially use the SD heightmap
	terrainShader = new Shader("../terrain.vert", "../terrain.frag");
	terrain = new Terrain("res/SanDiegoTerrain.ppm");

	//let there be light!
	sun = new Light(Global::lightPosition, Global::lightColor);

	//initialize the player camera
	camera = new Camera(Global::cam_pos_init, Global::cam_look_at_init, Global::cam_up_init);

	//Add Targets
	Target* targ = new Target();
	collidingObjects.push_back(targ);

	showWires = -1;
	demoMode = -1;
}

void Window::clean_up()
{
	delete(skybox);
	delete(skyboxShader);
	delete(particleSystem);
	delete(particleShader);
	delete(terrain);
	delete(terrainShader);
	delete(sun);
	delete(camera);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, Global::WINDOW_TITLE, NULL, NULL);

	//hide the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	lastTime = glfwGetTime();

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	}
}

void Window::idle_callback()
{	
	//compute the time difference between the current and last frame
	currentTime = glfwGetTime();
	deltaTime = float(currentTime - lastTime);

	//controlling the camera
	camera->update(c_moveDir, horizAngle, vertAngle, deltaTime);

	if (spin) {
		//horizAngle += 4 * Global::LOOK_SPEED;
	}
	//store the last frame time
	lastTime = currentTime;
	
	//Check for any Collisions
	collisionCheck();

	//Show wires
	for (int i = 0; i < collidingObjects.size(); i++) {
		if (showWires == 1)
			collidingObjects[i]->showBounding = true;
		else
			collidingObjects[i]->showBounding = false;
		if (collidingObjects[i]->colliding && Global::isFiring && demoMode == -1)
			collidingObjects[i]->alive = false;
	}
}

int GetIntersection(float fDst1, float fDst2, glm::vec3 P1, glm::vec3 P2, glm::vec3 &Hit) {
	if ((fDst1 * fDst2) >= 0.0f) return 0;
	if (fDst1 == fDst2) return 0;
	Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
	return 1;
}

int InBox(glm::vec3 Hit, glm::vec3 B1, glm::vec3 B2, const int Axis) {
	if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
	if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
	if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
	return 0;
}

void Window::collisionCheck() {

	glm::vec3 start = camera->getPos();
	int maxLength = 16;

	for (int i = 0; i < collidingObjects.size(); i++) {
		for (int j = 0; j < maxLength; j++) {

			glm::vec3 finish = (float)j * (camera->getDir()) + camera->getPos();

			//Start and end points of the line segment in box space
			glm::vec3 L1 = glm::vec3(glm::inverse(collidingObjects[i]->toWorld) * glm::vec4(start, 1));
			glm::vec3 L2 = glm::vec3(glm::inverse(collidingObjects[i]->toWorld) * glm::vec4(finish, 1));

			//Two vec3s that define the bounding box
			glm::vec3 B1 = glm::vec3(collidingObjects[i]->xmin, collidingObjects[i]->ymin, collidingObjects[i]->zmin);
			glm::vec3 B2 = glm::vec3(collidingObjects[i]->xmax, collidingObjects[i]->ymax, collidingObjects[i]->zmax);


			//Intersection point
			glm::vec3 Hit;

			//Checks if the line falls outside of the bounding box
			if (L2.x < B1.x && L1.x < B1.x) {
				collidingObjects[i]->colliding = false;
				continue;
			}
			if (L2.x > B2.x && L1.x > B2.x) {
				collidingObjects[i]->colliding = false;
				continue;
			}
			if (L2.y < B1.y && L1.y < B1.y) {
				collidingObjects[i]->colliding = false;
				continue;
			}
			if (L2.y > B2.y && L1.y > B2.y) {
				collidingObjects[i]->colliding = false;
				continue;
			}
			if (L2.z < B1.z && L1.z < B1.z) {
				collidingObjects[i]->colliding = false;
				continue;
			}
			if (L2.z > B2.z && L1.z > B2.z) {
				collidingObjects[i]->colliding = false;
				continue;
			}

			//Checks if the start point is inside of the bounding box
			if (L1.x > B1.x && L1.x < B2.x &&
				L1.y > B1.y && L1.y < B2.y &&
				L1.z > B1.z && L1.z < B2.z)
			{
				Hit = L1;
				collidingObjects[i]->colliding = true;
				collidingObjects[i]->hit = Hit;
				break;
			}

			//Finds the point of intersection
			if ((GetIntersection(L1.x - B1.x, L2.x - B1.x, L1, L2, Hit) && InBox(Hit, B1, B2, 1))
				|| (GetIntersection(L1.y - B1.y, L2.y - B1.y, L1, L2, Hit) && InBox(Hit, B1, B2, 2))
				|| (GetIntersection(L1.z - B1.z, L2.z - B1.z, L1, L2, Hit) && InBox(Hit, B1, B2, 3))
				|| (GetIntersection(L1.x - B2.x, L2.x - B2.x, L1, L2, Hit) && InBox(Hit, B1, B2, 1))
				|| (GetIntersection(L1.y - B2.y, L2.y - B2.y, L1, L2, Hit) && InBox(Hit, B1, B2, 2))
				|| (GetIntersection(L1.z - B2.z, L2.z - B2.z, L1, L2, Hit) && InBox(Hit, B1, B2, 3))) {
				collidingObjects[i]->colliding = true;
				collidingObjects[i]->hit = Hit;
				break;
			}

			collidingObjects[i]->colliding = false;
		}
	}
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the skybox
	skyboxShader->use();
	skybox->draw(skyboxShader->getProgram());

	//render the terrain
	terrainShader->use();
	terrain->render();

	//render the shooting particle effect
	//whether or not to actually render particles (i.e. when shooting) is handled behind the scenes
	particleShader->use();
	particleSystem->render(Global::camera->getPos() + Global::camOffset, Global::camera->getDir());

	//render the Targets
	for (int i = 0; i < collidingObjects.size(); i++) {
		collidingObjects[i]->draw();
	}

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);

}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{		
		//determine what key was pressed
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		
		//////// FPS CAMERA MOVEMENT ////////
		//move forward
		case GLFW_KEY_W:
			c_moveDir = C_FORWARD;
			break;

		//move backward
		case GLFW_KEY_S:
			c_moveDir = C_BACKWARD;
			break;

		//strafe right
		case GLFW_KEY_D:
			c_moveDir = C_RIGHT;
			break;

		//strafe left
		case GLFW_KEY_A:
			c_moveDir = C_LEFT;
			break;

		//Boolean Checks
		//Show wire frame
		case GLFW_KEY_T:
			showWires *= -1;
			break;

		//Have objects not die on hit
		case GLFW_KEY_G:
			demoMode *= -1;
			break;

		//Toggle glow
		case GLFW_KEY_I:
			for (int i = 0; i < collidingObjects.size(); i++) {
				if (collidingObjects[i]->glowing != 0.0f) {
					collidingObjects[i]->glowing = 0.0f;
				}
				else {
					collidingObjects[i]->glowing = 1.0f;
				}
			}
			break;

		//Reset
		case GLFW_KEY_R:
			for (int i = 0; i < collidingObjects.size(); i++)
				collidingObjects[i]->alive = true;
			break;

		case GLFW_KEY_P:
			spin = true;
			break;

		//////// REFORMAT THE TERRAIN ////////
		//grass
		case GLFW_KEY_1:
			Global::useHeightmap = false;
			Global::terrainTextureController->setTextures(GRASS);
			terrain->generateTerrain();
			break;

		//desert
		case GLFW_KEY_2:
			Global::useHeightmap = false;
			Global::terrainTextureController->setTextures(DESERT);
			terrain->generateTerrain();
			break;

		//snow
		case GLFW_KEY_3:
			Global::useHeightmap = false;
			Global::terrainTextureController->setTextures(SNOW);
			terrain->generateTerrain();
			break;

		//go back to using heightmap
		case GLFW_KEY_H:
			Global::useHeightmap = true;
			terrain->generateTerrain();

		default:
			std::cerr << "Unmapped key press" << std::endl;
			break;
		}

	}

	//check for a key release
	else if (action == GLFW_RELEASE)
	{
		c_moveDir = C_NONE;
	}
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//update the mouse position
	mouseX = xpos;
	mouseY = ypos;

	//reset mouse position for next frame
	glfwSetCursorPos(window, width / 2, height / 2);

	//compute the new orientation
	horizAngle += Global::LOOK_SPEED * float(width / 2 - mouseX);
	vertAngle += Global::LOOK_SPEED * float(height / 2 - mouseY);
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//mouse button pressed
	if (action == GLFW_PRESS)
	{
		//determine which button was pressed
		switch(button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			//get the coordinates of where the button was clicked
			glfwGetCursorPos(window, &mouseX, &mouseY);
			//start shooting
			Global::isFiring = true;
			break;
		}
	}

	//mouse button released
	else if (action == GLFW_RELEASE)
	{
		//stop shooting when the button is released
		Global::isFiring = false;
	}

}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}
