#pragma once
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
#include "gl_error_handling.h"

bool GPULoadTexture(const unsigned char * data, GLuint width, GLuint height, GLuint* id);

bool GPULoadUITexture(const unsigned char * data, GLuint width, GLuint height, GLuint* id);