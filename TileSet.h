#pragma once
#include <string> // memcpy
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "GLTextureHelper.h"
#include <vector>
#include "Rendering.h"
class TileSet
{
public:
	TileSet();
	~TileSet();
	void getTileBytes(unsigned int tilenum, unsigned char * output);
	void tileDirectMemCpy(unsigned int tilenum, TileSet& tileset, unsigned char* output);
	std::string         imgpath;
	std::string         name;
	unsigned int        columns;
	unsigned int        imgwidth;
	unsigned int        imgheight;
	unsigned int        margin;
	unsigned int        spacing;
	unsigned int        tilewidth;
	unsigned int        tileheight;
	unsigned int        tilecount;
	unsigned char*      imgdata;
	void                freeData();
	GLuint              texID = 0;
	void                genTexture();
	void                genSprites();
	Sprite*             getSprite(int i) { return &sprites[i]; }
private:
	std::vector<Sprite> sprites;
};

