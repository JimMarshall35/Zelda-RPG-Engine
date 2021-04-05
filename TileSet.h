#pragma once
#include <string> // memcpy
class TileSet
{
public:
	TileSet();
	~TileSet();
	void getTileBytes(unsigned int tilenum, unsigned char * output);
	void tileDirectMemCpy(unsigned int tilenum, TileSet& tileset, unsigned char* output);
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

