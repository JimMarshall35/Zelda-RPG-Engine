#pragma once
#include "TileLayer.h"
#include "GameObject.h"
#include <vector>
#include <queue>
class Area
{
public:
	friend class AreaLoader;
	friend class Game;
	Area();
	~Area() { freeData(); }
	void                     draw(const Shader& s, const Camera* camera);
	void                     update(float delta, GLuint keys);
	TileLayers*              getBackGround() { return &tilelayers; }
	TileSet*                 getTilesetByName(std::string name);
	Game*                    getGamePtr() { return game; }
	GameObject*              getPlayerPtr();
	std::vector<GameObject*> getEnemiesPtrs();
	
	void                     deleteAllGO();
	void                     enqueueForDelete(GameObject* go);
	void                     enqueueForCreate(GameObject* go);
private:
	TileSet*				 tilesets;
	unsigned int			 numtilesets;
	TileLayers				 tilelayers;
	void                     freeData();
	std::vector<GameObject*> gameobjects;
	void                     updatePhysics();
	bool                     AABBCollision(GameObject* go1, GameObject* go2);
	bool                     AABBCollision(GameObject * go1, GameObject * go2, glm::vec2 go1_vel, glm::vec2 go2_vel);
	Game*                    game;
	void                     deleteGO(GameObject* go);
	std::queue<GameObject*>  deleteQueue;
	std::queue<GameObject*>  createQueue;

};

