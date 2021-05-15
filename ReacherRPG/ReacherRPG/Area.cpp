#include "Area.h"

#include <algorithm>
#include <execution>

Area::Area()
{

}


bool compareGO(GameObject* go1, GameObject* go2) {
	return (go1->position.y - go1->scale.y > go2->position.y - go2->scale.y);
}

void Area::draw(const Shader & s, const Camera * camera)
{
	tilelayers.draw_bg(s, camera);
	// want to sort by y value in descending order - draw from top of window down
	std::sort(gameobjects.begin(), gameobjects.end(), compareGO);

	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		//std::cout << gameobjects[i]->position.y << std::endl;
		if (gameobjects[i]->isdrawable) {
			gameobjects[i]->draw(s, camera);
		}
	}
	tilelayers.draw_fg(s, camera);
	//std::cout << std::endl << std::endl;
}

void Area::update(float delta, GLuint keys)
{
	const int sample_size = 50;
	static int numcounts = 0;
	static double accumulated_total;
	std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < gameobjects.size(); i++) {
		gameobjects[i]->update(delta, keys);
	}
	//std::vector<GameObject*> go_copy;
	//go_copy = gameobjects;
	
	for (size_t i = 0; i < gameobjects.size(); i++) {
		GameObject* go1 = gameobjects[i];
		go1->position += go1->velocity;
		for (size_t j = i; j < gameobjects.size(); j++) {
			GameObject* go2 = gameobjects[j];
			if (go1 != go2 && go1 != nullptr && go2 != nullptr) {
				if (AABBCollision(go1, go2,go1->velocity,go2->velocity)) {
					if (go1->onInteract(go2)) return;
					if (go2->onInteract(go1)) return;
				}
			}
		}
	}
	
	updatePhysics();

	while (deleteQueue.size() > 0) {
		deleteGO(deleteQueue.front());
		deleteQueue.pop();
	}
	while (createQueue.size() > 0) {
		gameobjects.push_back(createQueue.front());
		createQueue.pop();
	}
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	accumulated_total += time_span.count();
	numcounts++;
	if (numcounts >= sample_size) {
		//std::cout << "update done in " << (accumulated_total/numcounts) * 1000 << " ms" << std::endl;
		numcounts = 0;
		accumulated_total = 0;
	}
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

GameObject * Area::getPlayerPtr()
{
	for (GameObject* p : gameobjects) {
		if (p->type == GO_TYPE::PLAYER) {
			return p;
		}
	}
	return nullptr;
}

std::vector<GameObject*> Area::getEnemiesPtrs()
{
	std::vector<GameObject*> returnvec;
	for (GameObject* go : gameobjects) {
		if (go->type == GO_TYPE::ENEMY) {
			returnvec.push_back(go);
		}
	}
	return returnvec;
}

void Area::deleteGO(GameObject * go)
{
	for (size_t i = 0; i < gameobjects.size(); i++) {
		GameObject* g = gameobjects[i];
		if (g == go) {
			gameobjects.erase(gameobjects.begin() + i);
			go->freeData();
			delete go;
			gameobjects.shrink_to_fit();
			return;
		}
	}
}

void Area::deleteAllGO()
{
	for (GameObject* g : gameobjects) {
		deleteGO(g);
	}
}

void Area::enqueueForDelete(GameObject * go)
{
	deleteQueue.push(go);
}

void Area::enqueueForCreate(GameObject * go)
{
	createQueue.push(go);
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
	tilelayers.freeData();
}

void Area::updatePhysics()
{
	for (size_t i = 0; i < gameobjects.size(); i++)
	{

		GameObject* go = gameobjects[i];

		if (go->issolidvsbackground) {
			
			float xstart = go->position.x + go->collider.left;
			float xend = go->position.x + go->collider.right;
			float ystart = go->position.y + go->collider.bottom;
			float yend = go->position.y + go->collider.top;

			for (float x = xstart; x <= xend; x += xend-xstart) {
				for (float y = ystart; y <= yend; y += yend-ystart) {
					int tile = tilelayers.getTileAtPosition(glm::vec2(x, y), "structure");
					if (tile > 0) {
						go->position += -go->velocity;
						goto nextGO;
					}
				}

			}
		}
nextGO:
		continue;
	}
}

bool Area::AABBCollision(GameObject * go1, GameObject * go2)
{
	float top1, bottom1, left1, right1;
	top1 = go1->position.y + go1->collider.top;
	left1 = go1->position.x + go1->collider.left;

	float top2, bottom2, left2, right2;
	top2 = go2->position.y + go2->collider.top;
	left2 = go2->position.x + go2->collider.left;
	
	Rect r1, r2;
	r1.x = left1;
	r1.y = top1;
	r1.w = go1->collider.width;
	r1.h = go1->collider.height;

	r2.x = left2;
	r2.y = top2;
	r2.w = go2->collider.width;
	r2.h = go2->collider.height;

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
	top1 = go1_newpos.y + go1->collider.top;
	left1 = go1_newpos.x + go1->collider.left;


	float top2, bottom2, left2, right2;
	top2 = go2_newpos.y + go2->collider.top;
	left2 = go2_newpos.x + go2->collider.left;

	Rect r1, r2;
	r1.x = left1;
	r1.y = top1;
	r1.w = go1->collider.width;
	r1.h = go1->collider.height;

	r2.x = left2;
	r2.y = top2;
	r2.w = go2->collider.width;
	r2.h = go2->collider.height;

	if (r1.x < r2.x + r2.w &&
		r1.x + r1.w > r2.x &&
		r1.y < r2.y + r2.h &&
		r1.y + r1.h > r2.y) {
		return true;
	}

	return false;
}