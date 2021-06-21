/**
  wrapper_glfw.cpp
  Modified from the OpenGL GLFW example to provide a wrapper GLFW class
  and to include shader loader functions to include shaders as text files
  Iain Martin August 2014
  */

#include "wrapper_glfw.h"

  /* Inlcude some standard headers */

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/* Constructor for wrapper object */
GLWrapper::GLWrapper(int width, int height, const char* title) {
	this->width = width;
	this->height = height;
	this->title = title;
	this->fps = 60;
	this->running = true;

	/* Initialise GLFW and exit if it fails */
	if (!glfwInit())
	{
		cout << "Failed to initialize GLFW." << endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
	glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	window = glfwCreateWindow(width, height, title, 0, 0);
	if (!window) {
		cout << "Could not open GLFW window." << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/* Obtain an OpenGL context and assign to the just opened GLFW window */
	glfwMakeContextCurrent(window);

	/* Initialise GLLoad library. You must have obtained a current OpenGL */
	if (!ogl_LoadFunctions())
	{
		cerr << "oglLoadFunctions() failed. Exiting" << endl;
		glfwTerminate();
		return;
	}

	/* Can set the Window title at a later time if you wish*/
	glfwSetWindowTitle(window, title);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, true);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

/* Terminate GLFW on destruvtion of the wrapepr object */
GLWrapper::~GLWrapper() {
	glfwTerminate();
}

/* Returns the GLFW window handle, required to call GLFW functions outside this class */
GLFWwindow* GLWrapper::getWindow()
{
	return window;
}

/*
 * Print OpenGL Version details
 */
void GLWrapper::DisplayVersion()
{
	/* One way to get OpenGL version*/
	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MAJOR_VERSION, &minor);
	cout << "OpenGL Version = " << major << "." << minor << endl;

	/* A more detailed way to the version strings*/
	cout << "Vender: " << glGetString(GL_VENDOR) << endl;
	cout << "Version:" << glGetString(GL_VERSION) << endl;
	cout << "Renderer:" << glGetString(GL_RENDERER) << endl;
}

/*
GLFW_Main function normally starts the windows system, calls any init routines
and then starts the event loop which runs until the program ends
*/
int GLWrapper::eventLoop()
{
	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Call function to draw your graphics
		renderer();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

/* Register an error callback function */
void GLWrapper::setErrorCallback(void(*func)(int error, const char* description))
{
	glfwSetErrorCallback(func);
}

/* Register a display function that renders in the window */
void GLWrapper::setRenderer(void(*func)()) {
	this->renderer = func;
}

/* Register a callback that runs after the window gets resized */
void GLWrapper::setReshapeCallback(void(*func)(GLFWwindow* window, int w, int h)) {
	glfwSetFramebufferSizeCallback(window, func);
}

/* Register a callback to respond to keyboard events */
void GLWrapper::setKeyCallback(void(*func)(GLFWwindow* window, int key, int scancode, int action, int mods))
{
	glfwSetKeyCallback(window, func);
}

/* Register a callback to respond to mouse darf events */
void GLWrapper::setMouseCallback(void(*func)(GLFWwindow* window, double xpos, double ypos))
{
	glfwSetCursorPosCallback(window, func);
}

void GLWrapper::setWindowFocusCallback(void(*func)(GLFWwindow* window, int focused)) {
	glfwSetWindowFocusCallback(window, func);
}