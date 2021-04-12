#include "Area.h"

#include <algorithm>
#include <execution>

Area::Area()
{

}


bool compareGO(GameObject* go1, GameObject* go2) {
	return (go1->position.y - go1->scale.y > go2->position.y - go2->scale.y);
}

void Area::draw(Shader & s, const Camera * camera)
{
	background.draw(s, camera);
	// want to sort by y value in descending order - draw from top of window down
	std::sort(gameobjects.begin(), gameobjects.end(), compareGO);

	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		//std::cout << gameobjects[i]->position.y << std::endl;
		if (gameobjects[i]->isdrawable) {
			gameobjects[i]->draw(s, camera);
		}
	}
	//std::cout << std::endl << std::endl;
}

void Area::update(float delta, GLuint keys)
{
	for (size_t i = 0; i < gameobjects.size(); i++) {
		gameobjects[i]->update(delta, keys);
	}
	for (size_t i = 0; i < gameobjects.size(); i++) {
		GameObject* go1 = gameobjects[i];
		for (size_t j = 0; j < gameobjects.size(); j++) {
			GameObject* go2 = gameobjects[j];
			if (go1 != go2) {
				if (AABBCollision(go1, go2)) {
					go1->onInteract(go2);
				}
			}
			
		}
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
		if (go->issolidvsgameobjects) {
			for (size_t j = 0; j < gameobjects.size(); j++) {
				GameObject* go2 = gameobjects[j];
				if (go != go2) {
					if (AABBCollision(go, go2)) {
						go->position -= go->velocity;
					}
				}

			}
		}
		if (go->issolidvsbackground) {
			float top, bottom, left, right;
			top = go->position.y + go->scale.y;
			bottom = go->position.y - go->scale.y;
			left = go->position.x - go->scale.x;
			right = go->position.x + go->scale.x;
			float resolutionx = go->scale.x / go->collider.pixelswidth;
			float resolutiony = go->scale.y / go->collider.pixelsheight;
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

bool Area::AABBCollision(GameObject * go1, GameObject * go2)
{
	float top1, bottom1, left1, right1;
	top1 = (go1->position.y + go1->scale.y) - (go1->collider.top_offset / go1->collider.pixelsheight) * 2.0f * go1->scale.y;
	bottom1 = (go1->position.y - go1->scale.y) + (go1->collider.bottom_offset / go1->collider.pixelsheight) * 2.0f * go1->scale.y;
	left1 = (go1->position.x - go1->scale.x) + (go1->collider.left_offset / go1->collider.pixelswidth) * 2.0f * go1->scale.x;
	right1 = (go1->position.x + go1->scale.x) - (go1->collider.right_offset / go1->collider.pixelswidth) * 2.0f * go1->scale.x;


	float top2, bottom2, left2, right2;
	top2 = (go2->position.y + go2->scale.y) - (go2->collider.top_offset / go2->collider.pixelsheight) * 2.0f * go2->scale.y;
	bottom2 = (go2->position.y - go2->scale.y) + (go2->collider.bottom_offset / go2->collider.pixelsheight) * 2.0f * go2->scale.y;
	left2 = (go2->position.x - go2->scale.x) + (go2->collider.left_offset / go2->collider.pixelswidth) * 2.0f * go2->scale.x;
	right2 = (go2->position.x + go2->scale.x) - (go2->collider.right_offset / go2->collider.pixelswidth) * 2.0f * go2->scale.x;
	
	Rect r1, r2;
	r1.x = left1;
	r1.y = top1;
	r1.w = right1 - left1;
	r1.h = top1 - bottom1;

	r2.x = left2;
	r2.y = top2;
	r2.w = right2 - left2;
	r2.h = top2 - bottom2;

	if (r1.x < r2.x + r2.w &&
		r1.x + r1.w > r2.x &&
		r1.y < r2.y + r2.h &&
		r1.y + r1.h > r2.y) {
		return true;
	}

	return false;
}

bool Area::AABBCollision(GameObject * go1, GameObject * go2, glm::vec2 go1_vel, glm::vec2 go2_vel)
{
	float top1, bottom1, left1, right1;
	glm::vec2 go1_newpos = go1->position + go1_vel;
	glm::vec2 go2_newpos = go2->position + go2_vel;
	top1 = (go1_newpos.y + go1->scale.y) - (go1->collider.top_offset / go1->collider.pixelsheight) * 2.0f * go1->scale.y;
	bottom1 = (go1_newpos.y - go1->scale.y) + (go1->collider.bottom_offset / go1->collider.pixelsheight) * 2.0f * go1->scale.y;
	left1 = (go1_newpos.x - go1->scale.x) + (go1->collider.left_offset / go1->collider.pixelswidth) * 2.0f * go1->scale.x;
	right1 = (go1_newpos.x + go1->scale.x) - (go1->collider.right_offset / go1->collider.pixelswidth) * 2.0f * go1->scale.x;


	float top2, bottom2, left2, right2;
	top2 = (go2_newpos.y + go2->scale.y) - (go2->collider.top_offset / go2->collider.pixelsheight) * 2.0f * go2->scale.y;
	bottom2 = (go2_newpos.y - go2->scale.y) + (go2->collider.bottom_offset / go2->collider.pixelsheight) * 2.0f * go2->scale.y;
	left2 = (go2_newpos.x - go2->scale.x) + (go2->collider.left_offset / go2->collider.pixelswidth) * 2.0f * go2->scale.x;
	right2 = (go2_newpos.x + go2->scale.x) - (go2->collider.right_offset / go2->collider.pixelswidth) * 2.0f * go2->scale.x;

	Rect r1, r2;
	r1.x = left1;
	r1.y = top1;
	r1.w = right1 - left1;
	r1.h = top1 - bottom1;

	r2.x = left2;
	r2.y = top2;
	r2.w = right2 - left2;
	r2.h = top2 - bottom2;

	if (r1.x < r2.x + r2.w &&
		r1.x + r1.w > r2.x &&
		r1.y < r2.y + r2.h &&
		r1.y + r1.h > r2.y) {
		return true;
	}

	return false;
}