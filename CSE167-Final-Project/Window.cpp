#include "window.h"
#include "globals.h"
#include "objects.h"

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

	//store the last frame time
	lastTime = currentTime;
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