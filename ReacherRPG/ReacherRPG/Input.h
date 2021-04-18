#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define UP_BIT 0
#define DOWN_BIT 1
#define LEFT_BIT 2
#define RIGHT_BIT 3
#define ZOOM_IN_BIT 4
#define ZOOM_OUT_BIT 5
#define SELECT_BIT 6

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
static GLuint keys = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
	case GLFW_KEY_W:
		if (action == GLFW_PRESS) {
			keys |= (1 << UP_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << UP_BIT);
		}
		break;
	case GLFW_KEY_S:
		if (action == GLFW_PRESS) {
			keys |= (1 << DOWN_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << DOWN_BIT);
		}
		break;
	case GLFW_KEY_A:
		if (action == GLFW_PRESS) {
			keys |= (1 << LEFT_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << LEFT_BIT);
		}
		break;
	case GLFW_KEY_D:
		if (action == GLFW_PRESS) {
			keys |= (1 << RIGHT_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << RIGHT_BIT);
		}
		break;
	case GLFW_KEY_Z:
		if (action == GLFW_PRESS) {
			keys |= (1 << ZOOM_OUT_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << ZOOM_OUT_BIT);
		}
		break;
	case GLFW_KEY_X:
		if (action == GLFW_PRESS) {
			keys |= (1 << ZOOM_IN_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << ZOOM_IN_BIT);
		}
		break;
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) {
			keys |= (1 << SELECT_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << SELECT_BIT);
		}
		break;
	case GLFW_KEY_1:
		break;
	}
	
	
}
GLuint getKeys() { return keys; }
void inputInit(GLFWwindow* window) {
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);
}
