#include "UI.h"
#include <vector>


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
	sprite_renderer.loadUISprite("Spritesheet/heart pixel art 32x32.png", "heart");
	sprite_renderer.loadUISprite("Spritesheet/msg_box_3.png", "msgbox");
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
	sprite_renderer.RenderSprite("heart", 25.0, 960, 1);
	sprite_renderer.RenderSprite("heart", 57, 960, 1);
	sprite_renderer.RenderSprite("heart", 89, 960, 1);
	renderMsgBox("hello world! The quick brown fox jumped over the lazy dog. Good game mate - when's it coming out? Can i get it on steam? No?!");
}

void UI::freeData()
{
	text_renderer.freeData();
	sprite_renderer.freeData();
}
#define ASCII_SPACE  0x20

void UI::renderMsgBox(std::string Contents)
{
	// this function will take a message box struct as its argument.
	// there will be a function that splits a string of whatever arbitrary length into a series of message box structs using the method below
	// to split into lines, then when the message box height has been reached move onto a new message box struct.
	// there will be some high level "Game" class that will contain a queue of these message box structs and pauses player input while 
	// the queue is not empty, input will de-queue the next message box and cause it to be rendered. UI class will only have a pointer to a single message box
	// - the one currently being rendered
	const float text_x_offset = 25;
	const float text_y_offset = -50;
	const float msgbox_scale = 2.5;
	const float msgbox_bottom_offset = 50;
	const float text_scale = 0.5f;
	unsigned int base_text_height = text_renderer.getBaseTextHeight();
	glm::ivec2 msgbox_size = sprite_renderer.getSpriteSize("msgbox");
	std::vector<std::string> lines;
	unsigned int string_length = text_renderer.geTextWidth(Contents) *text_scale;

	float accumulated_length = 0;
	std::string::const_iterator c;
	c = Contents.begin();
	std::string string_being_built;
	while (c != Contents.end())
	{
		string_being_built += *c;
		float width = text_renderer.geTextWidth(string_being_built) * text_scale;
		if (width >= (msgbox_size.x*msgbox_scale) - 2*text_x_offset) {
			lines.push_back(string_being_built);
			char next = ASCII_SPACE;
			if (c + 1 != Contents.end()) {
				next = *(c + 1);
			}
			if (*c != ASCII_SPACE && (next != ASCII_SPACE)) {
				string_being_built = "...";
			}
			else {
				string_being_built = "";
			}
		}

		c++;
	}
	lines.push_back(string_being_built);
	float xpos = 512 - (msgbox_scale * (msgbox_size.x / 2));
	float ypos = msgbox_size.y * msgbox_scale + msgbox_bottom_offset;
	sprite_renderer.RenderSprite("msgbox", xpos, ypos, msgbox_scale);
	for (size_t i = 0; i < lines.size(); i++) {
		std::string line = lines[i];
		text_renderer.RenderText(line, xpos + text_x_offset, ypos + text_y_offset - (i*base_text_height*text_scale), 0.5, glm::vec3(1.0, 1.0, 1.0));
	}
	
	
}
