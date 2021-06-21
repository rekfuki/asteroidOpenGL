/**
wrapper_glfw.h
Modified from the OpenGL GLFW example to provide a wrapper GLFW class
Iain Martin August 2014
*/
#pragma once

#include <string>

/* Inlcude GL_Load and GLFW */
#include <glload/gl_4_0.h>
#include <glload/gl_load.h>
#include <GLFW/glfw3.h>

class GLWrapper {
private:

	int width;
	int height;
	const char* title;
	double fps;
	void(*renderer)();
	bool running;
	GLFWwindow* window;

public:
	GLWrapper(int width, int height, const char* title);
	~GLWrapper();

	void setFPS(double fps) {
		this->fps = fps;
	}

	void DisplayVersion();

	/* Callback registering functions */
	void setRenderer(void(*f)());
	void setReshapeCallback(void(*f)(GLFWwindow* window, int w, int h));
	void setKeyCallback(void(*f)(GLFWwindow* window, int key, int scancode, int action, int mods));
	void setMouseCallback(void(*f)(GLFWwindow* window, double xpos, double ypos));
	void setWindowFocusCallback(void(*f)(GLFWwindow* window, int focused));
	void setErrorCallback(void(*f)(int error, const char* description));

	int eventLoop();
	GLFWwindow* getWindow();
};
