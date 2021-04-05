#include "Area.h"



Area::Area()
{

}




void Area::draw(Shader & s, const Camera * camera)
{
	background.draw(s, camera);
	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		if (gameobjects[i]->isdrawable) {
			gameobjects[i]->draw(s, camera);
		}
	}
}

void Area::update(float delta, GLuint keys)
{
	for (size_t i = 0; i < gameobjects.size(); i++) {
		gameobjects[0]->update(delta, keys);
	}
	updatePhysics();
}

TileSet * Area::getTilesetByName(std::string name)
{
	for (size_t i = 0; i < numtilesets; i++) {
		if (tilesets[i].name == name) {
			return &tilesets[i];
		}
	}
	return nullptr;
}

void Area::freeData()
{
	for (size_t i = 0; i < numtilesets; i++) {
		tilesets[i].freeData();
	}
	for (size_t i = 0; i < gameobjects.size(); i++) {
		delete gameobjects[i];
	}
	gameobjects.clear();
	gameobjects.shrink_to_fit();
	delete[] tilesets;
	background.freeData();
}

void Area::updatePhysics()
{
	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		GameObject* go = gameobjects[i];
		go->position += go->velocity;
		if (go->issolidvsbackground) {
			int tile = background.getTileAtPosition(go->position, "structure");
			if (tile > 0) {
				go->position += -go->velocity;
			}
		}
	}
}
