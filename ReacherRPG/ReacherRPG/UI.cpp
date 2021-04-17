#include "UI.h"



UI::UI()
{

}

UI::UI(std::string font)
{
	text_renderer.init(font);
	sprite_renderer.SetVAOandVBO(text_renderer.getVAO(), text_renderer.getVBO()); // need to make a third separate class that 
	                                                                              // contains the vao and vbo's used for ui rendering
	                                                                              // to prevent this weird  bit
	sprite_renderer.init();
	sprite_renderer.loadUISprite("Spritesheet/rune_tome.png", "rune_tome");
}
UI::~UI()
{
}

void UI::drawFPS(float delta)
{
	const int fpscount_period = 50;
	static int onfpscount = 0;
	static float fps_accumulated = 0;
	static float fps = 0;
	fps_accumulated += delta;
	onfpscount++;
	if (onfpscount >= fpscount_period) {
		fps = 1 / (fps_accumulated / fpscount_period);
		onfpscount = 0;
		fps_accumulated = 0;
	}
	std::string fps_str = std::to_string(fps).substr(0,6) + " fps";
	text_renderer.RenderText(fps_str, 25.0f, 970, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
	sprite_renderer.RenderSprite("rune_tome", 150, 150, 2);
}

void UI::freeData()
{
	text_renderer.freeData();
	sprite_renderer.freeData();
}
