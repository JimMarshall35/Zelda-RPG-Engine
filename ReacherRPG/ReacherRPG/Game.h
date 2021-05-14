#pragma once
#include "AreaLoader.h"
#include "UI.h"
#include <queue>
#include "inputdefs.h"
#include "Scripting.h"
class Game
{
public:
	Game();
	~Game();
	void loadArea(std::string folder, std::string file);
	void update(float delta, GLuint keys);
	void draw(const Shader& s, const Camera* c);
	void enqueueMsgBoxes(std::string text) { ui.emqueueMsgBoxes(text, msgboxQ); } // convert an arbitrary length string into a series of MessageBox structs and store in queue
	void freeData();
	Area* getAreaPtr() { return &area; }
private:
	std::queue<MessageBox> msgboxQ;
	Area                   area;
	UI                     ui;
	GLuint                 lastkeys;
	std::string            next_area_folder;
	std::string            next_area_file;
	bool                   should_load_next_area = false;
};

