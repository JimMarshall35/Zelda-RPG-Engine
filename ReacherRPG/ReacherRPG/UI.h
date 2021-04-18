#pragma once
#include "Rendering.h"
class UI
{
public:
	UI();
	UI(std::string font);
	~UI();
	void drawFPS(float delta);
	void freeData();
private:
	TextRenderer     text_renderer;
	UISpriteRenderer sprite_renderer;
	void             renderMsgBox(std::string Contents);
};

