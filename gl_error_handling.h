#pragma once
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <iostream>
static void GLClearErrors() {
	while (glGetError() != GL_NO_ERROR);
}
static void GLPrintErrors() {
	while (GLenum code = glGetError()) {
		std::cerr << "GL error (" << code << ")" << std::endl;
	}
}
static void GLPrintErrors(const char* errorlocation) {
	while (GLenum code = glGetError()) {
		std::cerr << "GL error (" << code << ") from " << errorlocation << std::endl;
	}
}