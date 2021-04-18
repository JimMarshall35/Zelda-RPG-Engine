#include "UI.h"
#include <vector>


UI::UI()
{
	text_renderer.init("fonts/Verdana Bold.ttf");
	sprite_renderer.SetVAOandVBO(text_renderer.getVAO(), text_renderer.getVBO()); // need to make a third separate class that 
																				  // contains the vao and vbo's used for ui rendering
																				  // to prevent this weird  bit
	sprite_renderer.init();
	sprite_renderer.loadUISprite("Spritesheet/heart pixel art 32x32.png", "heart");
	sprite_renderer.loadUISprite("Spritesheet/msg_box_2.png", "msgbox");
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

void UI::update(float delta)
{
	updateFPS(delta);
}

void UI::draw()
{
	drawFPS();
	renderMsgBox();
}

void UI::drawFPS()
{
	
	std::string fps_str = std::to_string(fps).substr(0,6) + " fps";
	text_renderer.RenderText(fps_str, 25.0f, 970, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
	sprite_renderer.RenderSprite("heart", 25.0, 960, 1);
	sprite_renderer.RenderSprite("heart", 57, 960, 1);
	sprite_renderer.RenderSprite("heart", 89, 960, 1);
	//renderMsgBox("hello world! The quick brown fox jumped over the lazy dog. Good game mate - when's it coming out? Can i get it on steam? No?!");
}

void UI::freeData()
{
	text_renderer.freeData();
	sprite_renderer.freeData();
}
#define ASCII_SPACE  0x20

void UI::renderMsgBox()
{

	if (currentMsgBox == nullptr) {
		return;
	}
	MessageBox m = *currentMsgBox;
	glm::ivec2 msgbox_size = sprite_renderer.getSpriteSize("msgbox");
	unsigned int base_text_height = text_renderer.getBaseTextHeight();
	float xpos = 512 - (m.msgbox_scale * (msgbox_size.x / 2));
	float ypos = msgbox_size.y * m.msgbox_scale + m.msgbox_bottom_offset;
	sprite_renderer.RenderSprite("msgbox", xpos, ypos, m.msgbox_scale);
	for (size_t i = 0; i < m.lines.size(); i++) {
		std::string line = m.lines[i];
		text_renderer.RenderText(line, xpos + m.text_x_offset, ypos - m.text_y_offset - (i*base_text_height*m.text_scale), m.text_scale, glm::vec3(1.0, 1.0, 0.0));
	}
	
	
}

void UI::emqueueMsgBoxes(std::string text, std::queue<MessageBox>& queue)
{

	MessageBox m;
	glm::ivec2 msgbox_size = sprite_renderer.getSpriteSize("msgbox");

	float accumulated_length = 0;
	std::string::const_iterator c;
	c = text.begin();
	std::string string_being_built;
	size_t i = 0;
	unsigned int height = 0;
	while (c != text.end())
	{
		string_being_built += *c;
		float width = text_renderer.geTextWidth(string_being_built) * m.text_scale;
		if(height >= (msgbox_size.y*m.msgbox_scale) - 2 * m.text_y_offset + text_renderer.getBaseTextHeight() * m.text_scale){
			//m.lines.push_back(string_being_built);
			queue.push(m);
			m.lines.clear();
			height = 0;
		}
		if (width >= (msgbox_size.x*m.msgbox_scale) - 2 * m.text_x_offset) {
			m.lines.push_back(string_being_built);
			i++;
			char next = ASCII_SPACE;
			if (c + 1 != text.end()) {
				next = *(c + 1);
			}
			if (*c != ASCII_SPACE && (next != ASCII_SPACE)) {
				string_being_built = "...";
			}
			else {
				string_being_built = "";
			}
			height += text_renderer.getBaseTextHeight()*m.text_scale;
		}
		c++;
	}
	m.lines.push_back(string_being_built);
	queue.push(m);
}

void UI::updateFPS(float delta)
{
	const int fpscount_period = 50;
	static int onfpscount = 0;
	static float fps_accumulated = 0;

	fps_accumulated += delta;
	onfpscount++;
	if (onfpscount >= fpscount_period) {
		fps = 1 / (fps_accumulated / fpscount_period);
		onfpscount = 0;
		fps_accumulated = 0;
	}
}
