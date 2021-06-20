#include "UI.h"
#include <vector>



UI::UI()
{
	init();
}
void UI::init() {

	// there will be an init function in lua script
	text_renderer.init("fonts/Final_Fantasy_VII.ttf"); // will be in lua script
	sprite_renderer.SetVAOandVBO(text_renderer.getVAO(), text_renderer.getVBO()); // need to make a third separate class that 
																				  // contains the vao and vbo's used for ui rendering
																				  // to prevent this weird  bit
	sprite_renderer.init();
	sprite_renderer.loadUISprite("Spritesheet/heart pixel art 32x32.png", "heart");// will be in lua script
	sprite_renderer.loadUISprite("Spritesheet/msg_box_3.png", "msgbox");// will be in lua script
	setupNormalUI();
}
UI::UI(std::string font)
{
	text_renderer.init(font);
	sprite_renderer.SetVAOandVBO(text_renderer.getVAO(), text_renderer.getVBO()); // need to make a third separate class that 
	                                                                              // contains the vao and vbo's used for ui rendering
	                                                                              // to prevent this weird  bit
	sprite_renderer.init();
	sprite_renderer.loadUISprite("Spritesheet/heart pixel art 32x32.png", "heart");// will be in lua script
	sprite_renderer.loadUISprite("Spritesheet/msg_box_3.png", "msgbox");// will be in lua script
	
}
UI::~UI()
{
}
#define MAX_HP 5
void UI::update(float delta, unsigned int keys)
{
	for (size_t i = 0; i < toDraw.size(); i++) {
		if (i >= player_HP) {
			toDraw[i].shouldDraw = false;
		}
		else {
			toDraw[i].shouldDraw = true;
		}
	}
	updateFPS(delta);
}

void UI::draw()// implementation NOT will be in lua script
{
	drawFPS();
	float startx = 25;
	float incr = 32;
	//for (size_t i = 0; i < player_HP; i++) {
	//	sprite_renderer.RenderSprite("heart", startx + i * incr, 960, 1);
	//}
	for (size_t i = 0; i < toDraw.size(); i++) {
		UISprite s = toDraw[i];
		if (toDraw[i].shouldDraw) {
			sprite_renderer.RenderSprite(s);
		}
	}
	renderMsgBox();
}

void UI::drawFPS()// will be in lua script
{
	std::string fps_str = std::to_string(fps).substr(0,6) + " fps";
	text_renderer.RenderText(fps_str, 25.0f, 970, 1.2f, glm::vec3(0.5, 0.8f, 0.2f));
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

void UI::setupNormalUI()
{
	float startx = 25;
	float incr = 32;
	for (size_t i = 0; i < MAX_HP; i++) {
		UISprite sprite;
		sprite.name  = "heart";
		sprite.x     = startx + i * incr;
		sprite.y     = 960;
		sprite.scale = 1;
		toDraw.push_back(sprite);
		//sprite_renderer.RenderSprite("heart", startx + i * incr, 960, 1);
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

void UI::onNotify(UIEvent msg)// implementation will be in lua script
{
	switch (msg.sendertype)
	{
	case GO_TYPE::PLAYER:
		if (msg.type == "health_set") {
			if (msg.data.checkInt()) {
				player_HP = msg.data.getInt();
			}
			
		}
		break;
	default:
		break;
	}

}

void UI::updateFPS(float delta)// will be in lua script
{
	const int fpscount_period = 25;
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
