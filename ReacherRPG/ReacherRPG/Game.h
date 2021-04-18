#pragma once
#include "AreaLoader.h"
#include "Area.h"
#include "UI.h"
#include <queue>
#define SELECT_BIT 6
class Game
{
public:
	Game();
	~Game();
	void loadArea(std::string folder, std::string file);
	void update(float delta, GLuint keys);
	void draw(Shader& s, const Camera* c);
	void enqueueMsgBoxes(std::string text) { ui.enqueMsgBoxes(text, msgboxQ); } // convert an arbitrary length string into a series of MessageBox structs and store in queue
	void freeData();
private:
	std::queue<MessageBox> msgboxQ;
	Area                   area;
	UI                     ui;
	GLuint                 lastkeys;
};

