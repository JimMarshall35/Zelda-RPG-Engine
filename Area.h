#pragma once
#include "Background.h"
#include "Player.h"
class Area
{
public:
	friend class AreaLoader;
	Area();
	~Area() { freeData(); }
	void draw(Shader& s, const Camera& camera);
	Background* getBackGround() { return &background; }
private:
	TileSet*           tilesets;
	unsigned int       numtilesets;
	Background         background;
	void               freeData();
	Player             player;
};

