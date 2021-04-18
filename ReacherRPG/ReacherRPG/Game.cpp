#include "Game.h"



Game::Game()
{
}


Game::~Game()
{
}

void Game::loadArea(std::string folder, std::string file)
{
	area.game = this;
	if (!AreaLoader::Instance()->loadArea(folder, file, area)) {
		std::cout << "failed to load level" << std::endl;
	}
}

void Game::update(float delta, GLuint keys)
{
	ui.update(delta);
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
	lastkeys = keys;
}

void Game::draw(Shader& s, const Camera * c)
{
	area.draw(s, c);
	ui.draw();
	
}

void Game::freeData()
{
	ui.freeData();
}
