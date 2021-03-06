#pragma once
#include <string> // memcpy
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "GLTextureHelper.h"
#include <vector>
#include "Rendering.h"
#include <map>
class TileSet
{
public:
	TileSet();
	~TileSet();
	void				getTileBytes(unsigned int tilenum, unsigned char * output);
	void				tileDirectMemCpy(unsigned int tilenum, unsigned char* output, unsigned int output_width);
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
	void                genMetaSprite(const unsigned int* tiles, const unsigned int width, const unsigned int numtiles, const std::string name);
	Sprite*             getNamedSprite(std::string name) { return &sprites[namedSpriteMap[name]]; }
private:
	std::vector<Sprite>            sprites;
	std::map<std::string, int>	   namedSpriteMap;
};

