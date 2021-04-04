#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void inputInit(GLFWwindow* window) {
	glfwSetWindowSizeCallback(window, window_size_callback);
}