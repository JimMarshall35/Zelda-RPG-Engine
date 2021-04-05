#pragma once
#include "Background.h"
#include "GameObject.h"
#include <vector>
class Area
{
public:
	friend class AreaLoader;
	Area();
	~Area() { freeData(); }
	void draw(Shader& s, const Camera* camera);
	void update(float delta, GLuint keys);
	Background* getBackGround() { return &background; }
	TileSet*    getTilesetByName(std::string name);
private:
	TileSet*				 tilesets;
	unsigned int			 numtilesets;
	Background				 background;
	void                     freeData();
	std::vector<GameObject*> gameobjects;
	void                     updatePhysics();
};

