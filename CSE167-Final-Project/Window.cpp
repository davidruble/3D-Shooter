#include "window.h"
#include "globals.h"
#include "objects.h"
#include "skybox.h"
#include "Camera.h"
#include "shader.h"

using Global::skybox;
using Global::camera;
using Global::skyboxShader;

//window datafields
int Window::width;
int Window::height;
glm::mat4 Window::P;

//objects in the scene
Skybox * Global::skybox;
Camera * Global::camera;
Shader * Global::skyboxShader;

CamTransform c_transop;

//last mouse click position
glm::vec3 lastPos = glm::vec3(0.0f, 0.0f, 0.0f);
float lastYPos = 0.0f;
float yDifference = 0.0f;

double mouseX = 0.0, mouseY = 0.0;
float zOffset = 0.0;

void Window::initialize_objects()
{
	// Load the shaders
	skyboxShader = new Shader("../skybox.vert", "../skybox.frag");

	camera = new Camera(Global::cam_pos_init, Global::cam_look_at_init, Global::cam_up_init);

	// Init the skybox
	skybox = new Skybox();
}

void Window::clean_up()
{
	delete(skybox);
	delete(skyboxShader);
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
	//controlling the camera
	camera->update(c_transop, (float) mouseX, (float) mouseY, (float) zOffset);

	//TODO: update the scene
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the skybox
	skyboxShader->use();
	skybox->draw(skyboxShader->getProgram());

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{		
		//determine what key was pressed
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		//TODO: WASD key presses move player (camera)

		default:
			std::cerr << "Unmapped key press" << std::endl;
			break;
		}
	}

	//check for a key release
	else if (action == GLFW_RELEASE)
	{
	}
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//update the mouse position
	mouseX = xpos;
	mouseY = ypos;
	lastPos = glm::vec3(mouseX, mouseY, 0.0f);
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//mouse button pressed
	if (action == GLFW_PRESS)
	{
		//determine which button was pressed
		switch(button)
		{
		// TODO: shoot gun on mouse press
		case GLFW_MOUSE_BUTTON_LEFT:
			//get the coordinates of where the button was clicked
			glfwGetCursorPos(window, &mouseX, &mouseY);
			break;
		}
	}

	//mouse button released
	else if (action == GLFW_RELEASE)
	{
	}

}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}