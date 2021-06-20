#include "UI.h"
#include <vector>



UI::UI()
{
	baseInit();// init lua vm
	init();    // init renderers 
	if (Scripting::checkLua(L, luaL_dofile(L, "scripts/ui.lua"))) {
		lua_pushlightuserdata(L, this);
		lua_setglobal(L, "host");
		registerLuaFunctions();
		lua_getglobal(L, "init");
		lua_call(L, 0, 0);
		//setupNormalUI();
	}
	
}
void UI::init() {

	// (text renderer initialized in its constructor)	
	sprite_renderer.SetVAOandVBO(text_renderer.getVAO(), text_renderer.getVBO()); // need to make a third separate class that 
																				  // contains the vao and vbo's used for ui rendering
																				  // to prevent this weird  bit
	sprite_renderer.init();
	
}
int UI::l_loadUISprite(lua_State * L)
{
	UI*         ptr  = (UI*)lua_touserdata(L, 1);
	std::string path = luaL_checkstring(L, 2);
	std::string name = luaL_checkstring(L, 3);
	ptr->sprite_renderer.loadUISprite(path, name);
	return 0;
}

int UI::l_loadFont(lua_State * L)
{
	UI*         ptr  = (UI*)lua_touserdata(L, 1);
	std::string path = luaL_checkstring(L, 2);
	ptr->text_renderer.loadFont(path);
	return 0;
}

int UI::l_pushToDraw(lua_State * L)
{
	UI* ptr = (UI*)lua_touserdata(L, 1);
	if (!lua_istable(L, 2)) { std::cerr << "2nd param l_pushToDraw not a table" << std::endl; return 0; }
	UISprite s;
	int type;
	// name
	type = lua_getfield(L, -1, "name");
	if (!ptr->checkTypeValue(type, LUA_TSTRING, "name")) { return 0; }
	s.name = lua_tostring(L, -1);
	// x
	type = lua_getfield(L, -2, "x");
	if (!ptr->checkTypeValue(type, LUA_TNUMBER, "x")) { return 0; }
	s.x = lua_tonumber(L, -1);
	// y
	type = lua_getfield(L, -3, "y");
	if (!ptr->checkTypeValue(type, LUA_TNUMBER, "y")) { return 0; }
	s.y = lua_tonumber(L, -1);
	// scale
	type = lua_getfield(L, -4, "scale");
	if (!ptr->checkTypeValue(type, LUA_TNUMBER, "scale")) { return 0; }
	s.scale = lua_tonumber(L, -1);
	// shoulddraw
	type = lua_getfield(L, -5, "shouldDraw");
	if (!ptr->checkTypeValue(type, LUA_TBOOLEAN, "shouldDraw")) { return 0; }
	s.shouldDraw = lua_toboolean(L, -1);

	// push sprite
	ptr->toDraw.push_back(s);
	return 0;
}

int UI::l_setToDraw(lua_State * L)
{
	/*
	Sets the array of UISprite structs which will be drawn 
	when UI::draw gets called

	struct UISprite {
		std::string name;
		float       x;
		float       y;
		float       scale;
		bool        shouldDraw = false;
	};
	*/
	UI*    ptr = (UI*)lua_touserdata(L, 1);
	// clear list to draw
	ptr->toDraw.clear();
	if (!lua_istable(L, 2)) {
		std::cerr << "lua argument 2 is not a table l_setToDraw" << std::endl;
	}
	size_t frames_size = lua_rawlen(L, 2);
	for (size_t i = 1; i <= frames_size; i++) { // lua starts at 1
		int type = lua_rawgeti(L, 2, i);
		UISprite s;
		if (type == LUA_TTABLE) {
			// name
			type = lua_getfield(L, -1, "name");
			if (!ptr->checkTypeValue(type, LUA_TSTRING, "name")) { return 0; }
			s.name = lua_tostring(L, -1);
			// x
			type = lua_getfield(L, -2, "x");
			if (!ptr->checkTypeValue(type, LUA_TNUMBER, "x")) { return 0; }
			s.x = lua_tonumber(L, -1);
			// y
			type = lua_getfield(L, -3, "y");
			if (!ptr->checkTypeValue(type, LUA_TNUMBER, "y")) { return 0; }
			s.y = lua_tonumber(L, -1);
			// scale
			type = lua_getfield(L, -4, "scale");
			if (!ptr->checkTypeValue(type, LUA_TNUMBER, "scale")) { return 0; }
			s.scale = lua_tonumber(L, -1);
			// shoulddraw
			type = lua_getfield(L, -5, "shouldDraw");
			if (!ptr->checkTypeValue(type, LUA_TBOOLEAN, "shouldDraw")) { return 0; }
			s.shouldDraw = lua_toboolean(L, -1);
			// clear stack
			lua_pop(L, 5);
			// push sprite
			ptr->toDraw.push_back(s);
		}
		else {
			std::cerr << "sprite at index " << i << " is not a table" << std::endl;
			return 0;
		}
	}
	return 0;
}

int UI::l_clearToDraw(lua_State * L)
{
	UI*    ptr = (UI*)lua_touserdata(L, 1);
	ptr->toDraw.clear();
	return 0;
}

UI::~UI()
{
}
#define MAX_HP 5
void UI::update(float delta, unsigned int keys)
{
	/*
	for (size_t i = 0; i < toDraw.size(); i++) {
		if (i >= player_HP) {
			toDraw[i].shouldDraw = false;
		}
		else {
			toDraw[i].shouldDraw = true;
		}
	}
	*/
	updateFPS(delta);
}

void UI::draw()// implementation NOT will be in lua script
{
	drawFPS();

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



void UI::registerLuaFunctions()
{
	                                                  
	registerFunction(l_loadUISprite, "loadUISprite"); // host, path, name => void
	registerFunction(l_loadFont,     "loadFont");     // host, font       => void
	registerFunction(l_setToDraw,    "setToDraw");    // host, UISprite[] => void
	registerFunction(l_clearToDraw,  "clearToDraw");  // host             => void 
	registerFunction(l_pushToDraw,   "pushToDraw");   // host,UISprite    => void
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
	lua_getglobal(L, "onNotify");
	if (!lua_isfunction(L, -1)) { std::cerr << "onNotify is not a function" << std::endl; return; }
	// make lua table
	lua_createtable(L, 0, 3);
	// set type field
	lua_pushstring(L, msg.type.c_str());
	lua_setfield(L, -2, "type");
	// set sendertype
	lua_pushinteger(L, (int)msg.sendertype);
	lua_setfield(L, -2, "sendertype");
	switch (msg.data.whichmember) {
	case UI_EVENT_BOOL:
		lua_pushboolean(L, msg.data.getBool());
		break;
	case UI_EVENT_FLOAT:
		lua_pushnumber(L, msg.data.getFloat());
		break;
	case UI_EVENT_INT:
		lua_pushinteger(L, msg.data.getInt());
		break;
	case UI_EVENT_STRING:
		lua_pushstring(L, msg.data.getString());
		break;
	default:
		std::cerr << "invalid ui event data type " << msg.data.whichmember;
		return;
	}
	lua_setfield(L, -2, "data");
	lua_call(L, 1, 0);
	/*
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
	*/
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
