#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "gl_error_handling.h"
#include "GLTextureHelper.h"

static float vertices[20] = {
	// position            tex coordinates
	1.0f,  1.0f, 0.0f,      1.0f, 0.0f,   // top right
	1.0f, -1.0f, 0.0f,      1.0f, 1.0f,   // bottom right
	-1.0f, -1.0f, 0.0f,     0.0f, 1.0f,   // bottom left
	-1.0f,  1.0f, 0.0f,     0.0f, 0.0f    // top left 
};
static const unsigned int indices[6] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

class Sprite {
public:
	Sprite();
	~Sprite() { }
	void setup_preexisting(GLuint tex_id,
		unsigned int img_width, unsigned int img_height,
		unsigned int sheet_x, unsigned int sheet_y, 
		unsigned int sheet_w, unsigned int sheet_h);              // for use with existing opengl texture

	void setup(unsigned char* img_data, 
		unsigned int img_width, unsigned int img_height, 
		unsigned int sheet_x, unsigned int sheet_y, 
		unsigned int sheet_w, unsigned int sheet_h);    // creates new texture from img data as well 

	void freeData();
	void draw(const glm::vec2 pos, const glm::vec3 scale, Shader& s, const Camera* camera);
	GLuint getPixelsHeight() { return height_pixels; }
	GLuint getPixelsWidth() { return width_pixels; }
private:
	void setup_base(unsigned int img_width, unsigned int img_height, unsigned int sheet_x, unsigned int sheet_y, unsigned int sheet_w, unsigned int sheet_h);
	GLuint VAO;

	GLuint TextureID;
	GLuint width_pixels;
	GLuint height_pixels;
	void genbuffers();
	
};

