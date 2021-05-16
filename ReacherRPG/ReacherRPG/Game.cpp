#include "Game.h"



Game::Game()
{
}


Game::~Game()
{
}

void Game::loadArea(std::string folder, std::string file)
{
	next_area_folder = folder;
	next_area_file = file;
	should_load_next_area = true;
}

void Game::update(float delta, GLuint keys)
{
	ui.update(delta,keys);
	if (msgboxQ.size() == 0) {
		area.update(delta, keys);
	}
	else {
		if (keys & (1 << SELECT_BIT) && !(lastkeys & (1 << SELECT_BIT))) {
			msgboxQ.pop();
		}
		if (msgboxQ.size() > 0) {
			ui.setMsgBox(&msgboxQ.front());
		}
		else {
			ui.setMsgBox(nullptr);
		}
		
		area.update(delta, 0);
	}
	if (should_load_next_area) {
		should_load_next_area = false;
		area.game = this;
		if (!AreaLoader::Instance()->loadArea(next_area_folder, next_area_file, area)) {
			std::cout << "failed to load level" << std::endl;
		}
		std::cout << area.gameobjects.size() << " game objects" << std::endl;
	}
	lastkeys = keys;
}

void Game::draw(const Shader& s, const Camera * c)
{
	area.draw(s, c);
	ui.draw();
	
}

void Game::freeData()
{
	ui.freeData();
}
