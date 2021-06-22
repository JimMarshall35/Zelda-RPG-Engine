#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "inputdefs.h"

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
static GLuint keys = 0;
GLuint getKeys() { return keys; }
static bool   joystick_present;
static bool   getJoystick() { return joystick_present; }
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	switch (key) {
	case GLFW_KEY_P:
		if (action == GLFW_PRESS) {
			keys |= (1 << PAUSE_BIT);
		}
		else if (action == GLFW_RELEASE) {
			keys &= ~(1 << PAUSE_BIT);
		}
		break;
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

void joystick_callback(int jid, int event)
{
	if (jid != GLFW_JOYSTICK_1)
		return;
	if (event == GLFW_CONNECTED)
	{
		std::cout << "joystick 1 connect!!" << std::endl;
		joystick_present = true;
	}
	else if (event == GLFW_DISCONNECTED)
	{
		std::cout << "joystick 1 disconnect :(" << std::endl;
		joystick_present = false;
	}
}
void inputInit(GLFWwindow* window) {
	glfwSetWindowSizeCallback(window, window_size_callback);
	
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (present == GLFW_TRUE) {
		std::cout << "joystick 1 present!!" << std::endl;
		joystick_present = true;
	}
	else {
		std::cout << "joystick 1 not present :(" << std::endl;
		joystick_present = false;
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetJoystickCallback(joystick_callback);
}


void pollController(GLFWwindow* window) {
	int count;
	keys = 0;
	const unsigned char* states = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
	for (size_t i = 0; i < count; i++) {
		switch (states[i]) {
		case GLFW_PRESS:
			//std::cout << i << std::endl;
			switch (i) {
			case XBOX_LEFT:
				keys |= (1 << LEFT_BIT);
				break;
			case XBOX_RIGHT:
				keys |= (1 << RIGHT_BIT);
				break;
			case XBOX_UP:
				keys |= (1 << UP_BIT);
				break;
			case XBOX_DOWN:
				keys |= (1 << DOWN_BIT);
				break;
			case XBOX_B:
			case XBOX_A:
				keys |= (1 << SELECT_BIT);
				break;
			case XBOX_START:
				keys |= (1 << PAUSE_BIT);
				break;
			case XBOX_LB:
				keys |= (1 << ZOOM_OUT_BIT);
				break;
			case XBOX_RB:
				keys |= (1 << ZOOM_IN_BIT);
				break;
			}
			break;
		}

	}
}
