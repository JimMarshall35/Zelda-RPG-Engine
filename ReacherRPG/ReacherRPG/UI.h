#pragma once
#include "Rendering.h"
#include <queue>
\
struct MessageBox {                         
	std::vector<std::string> lines;
	float text_x_offset = 35;
	float text_y_offset = 65;
	float msgbox_scale = 3;
	float msgbox_bottom_offset = 100;
	float text_scale = 1.2f;
	float xpos;
	float ypos;
};

class UI
{
public:
	UI();
	UI(std::string font);
	~UI();
	void             update(float delta);
	void             draw();
	void             drawFPS();
	void             freeData();
	void             emqueueMsgBoxes(std::string text, std::queue<MessageBox>& queue);
	void             setMsgBox(MessageBox* m) { currentMsgBox = m; }
private:
	void             updateFPS(float delta);
	TextRenderer     text_renderer;
	UISpriteRenderer sprite_renderer;
	void             renderMsgBox();
	MessageBox*      currentMsgBox = nullptr;
	float            fps;
	
};

