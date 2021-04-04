#pragma once
#include <vector>
#include <string>
#include "stb_image.h"
// Include GLEW
#include "Shader.h"
#include "gl_error_handling.h"
#include "Camera.h"

#define VERTICES_SIZE 20
#define FLOATS_PER_VERTEX 5

static float vertices[] = {
	// position            tex coordinates
	1.0f,  1.0f, 0.0f,      1.0f, 0.0f,   // top right
	1.0f, -1.0f, 0.0f,      1.0f, 1.0f,   // bottom right
	-1.0f, -1.0f, 0.0f,     0.0f, 1.0f,   // bottom left
	-1.0f,  1.0f, 0.0f,     0.0f, 0.0f    // top left 
};
static const unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};
struct TileSet {
	std::string     imgpath;
	std::string     name;
	unsigned int    columns;
	unsigned int    imgwidth;
	unsigned int    imgheight;
	unsigned int    margin;
	unsigned int    spacing;
	unsigned int    tilewidth;
	unsigned int    tileheight;
	unsigned int    tilecount;
	unsigned char*  imgdata;
};

struct BG_Layer {
	std::string          name;
	unsigned int*        tiles;
	unsigned int         width;
	unsigned int         height;
	GLuint               textureID;         
};
class Background
{
public:
	friend class AreaLoader;
	Background();
	~Background() { freeData(); }
	void debugPrint();
	void draw(Shader& s, const Camera& camera);
	glm::vec2      get_base_scale() { return glm::vec2(base_scale); }
	void           freeData();
private:
	BG_Layer*      layers;
	void           getTileBytes(unsigned int tilenum, TileSet& tileset, unsigned char* output);
	void           tileDirectMemCpy(unsigned int tilenum, TileSet& tileset, unsigned char* output);
	void           genLayersTextures(TileSet& tileset);
	void           genBuffers();
	GLuint         VAO;
	unsigned int   numlayers;
	unsigned int   numtilesets;
	unsigned int   width;
	unsigned int   height;
	unsigned int   tilewidth;
	unsigned int   tileheight;
	void           setInitialScale();
	glm::vec3      base_scale;
	glm::vec2      position = glm::vec2(0.0);
};

