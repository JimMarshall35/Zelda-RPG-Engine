#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "gl_error_handling.h"
#include "GLTextureHelper.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H  


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


static const float Full_img_vertices[20] = {
	// position            tex coordinates
	1.0f,  1.0f, 0.0f,      1.0f, 0.0f,   // top right
	1.0f, -1.0f, 0.0f,      1.0f, 1.0f,   // bottom right
	-1.0f, -1.0f, 0.0f,     0.0f, 1.0f,   // bottom left
	-1.0f,  1.0f, 0.0f,     0.0f, 0.0f    // top left 
};
static const unsigned int full_img_indices[6] = {  // note that we start from 0!
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

	void setup(unsigned char* img_data, unsigned int img_width, unsigned int img_height);
	void setup_preexisting(GLuint tex_id, unsigned int img_width, unsigned int img_height);


	void freeData();
	void draw(const glm::vec2 pos, const glm::vec3 scale, const Shader& s, const Camera* camera);
	GLuint getPixelsHeight() { return height_pixels; }
	GLuint getPixelsWidth() { return width_pixels; }
	static void generateFullImgVAO();
	static void freeFullImgVAO();
private:
	void setup_base(unsigned int img_width, unsigned int img_height, unsigned int sheet_x, unsigned int sheet_y, unsigned int sheet_w, unsigned int sheet_h);
	GLuint VAO;

	GLuint TextureID;
	GLuint width_pixels;
	GLuint height_pixels;
	void genbuffers();
	static GLuint FullImgVAO;

};

class TextRenderer {
	// code mostly copy and pasted from learnopengl.com section on text rendering and made into this class 
public:
	TextRenderer();
	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
	void loadFont(std::string font);
	GLuint getVAO() { return VAO; }
	GLuint getVBO() { return VBO; }
	void freeData();
	unsigned int geTextWidth(std::string text);
	unsigned int getBaseTextHeight() { return base_text_height; }
private:
	const unsigned int base_text_height = 48;
	void genBuffers();
	struct Character {
		unsigned int TextureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Offset to advance to next glyph - (in 1/64th pixels)
	};
	std::map<char, Character> Characters;
	
	GLuint VAO, VBO;
	Shader shader;
};
struct UISprite {
	std::string name;
	float       x;
	float       y;
	float       scale;
	bool        shouldDraw = false;
};
class UISpriteRenderer {
public:
	void RenderSprite(std::string name, float x, float y, float scale);
	void RenderSprite(UISprite s) { RenderSprite(s.name, s.x, s.y, s.scale); }
	void SetVAOandVBO(GLuint pVAO, GLuint pVBO) { VAO = pVAO; VBO = pVBO; }
	bool loadUISprite(std::string img_path, std::string name);
	void init();
	void freeData();
	glm::ivec2 getSpriteSize(std::string name) { return sprites[name].Size; }
private:
	struct UISprite {
		unsigned int TextureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	};
	std::map<std::string, UISprite> sprites;
	GLuint VAO, VBO;
	Shader shader;
};

void initRendering();
void cleanupRendering();