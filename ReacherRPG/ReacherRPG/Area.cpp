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
		gameobjects[i]->freeData();
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
			float top, bottom, left, right;
			top = go->position.y + go->scale.y;
			bottom = go->position.y - go->scale.y;
			left = go->position.x - go->scale.x;
			right = go->position.x + go->scale.y;
			float resolutionx = go->scale.x / go->collider.resolution;
			float resolutiony = go->scale.y / go->collider.resolution;
			float xstart = left + go->collider.left_offset * resolutionx;
			float xend = right - go->collider.right_offset * resolutionx;
			float ystart = bottom + go->collider.bottom_offset * resolutiony;
			float yend = top - go->collider.top_offset * resolutiony;
			for (float x = xstart; x < xend; x += resolutionx) {
				for (float y = ystart; y < yend; y += resolutiony) {
					int tile = background.getTileAtPosition(glm::vec2(x, y), "structure");
					if (tile > 0) {
						go->position += -go->velocity;
						goto nextGO;
					}
				}

			}
nextGO:     
			continue;
		}
	}
}
