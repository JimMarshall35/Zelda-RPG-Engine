#pragma once
#include "Rendering.h"
#include <queue>
#include "GameObject.h"

#define MAX_HP 5
class Game;
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

class UIMessageData {
#define UI_EVENT_INT    0
#define UI_EVENT_STRING 1
#define UI_EVENT_FLOAT  2
#define UI_EVENT_BOOL   3

public:
	inline void setInt(int i) {
		whichmember = UI_EVENT_INT;
		u.i = i;
	}
	inline void setString(const char* s) {
		whichmember = UI_EVENT_STRING;
		u.s = s;
	}
	inline void setFloat(float f) {
		whichmember = UI_EVENT_FLOAT;
		u.f = f;
	}
	inline void setBool(bool b) {
		whichmember = UI_EVENT_BOOL;
		u.b = b;
	}
	inline bool        checkInt()    { return whichmember == UI_EVENT_INT; }
	inline bool        checkString() { return whichmember == UI_EVENT_STRING; }
	inline bool        checkFloat()  { return whichmember == UI_EVENT_FLOAT; }
	inline bool        checkBool()   { return whichmember == UI_EVENT_BOOL; }

	inline const char* getString()   { return u.s; }
	inline int         getInt()      { return u.i; }
	inline float       getFloat()    { return u.f; }
	inline bool        getBool()     { return u.b; }
	int                whichmember;
private:
	
	union {
		int         i;
		const char* s;
		float       f;
		bool        b;
	}u;
};
enum class GO_TYPE;
struct UIEvent {
	std::string   type;
	GO_TYPE       sendertype;
	UIMessageData data;
	
};

class UI : public Scripting::LuaVMBase
{
public:
	//friend class Game;
	UI();
	~UI();
	void             update(float delta, unsigned int keys); 
	void             draw();
	void             drawFPS();
	void             freeData();
	void             emqueueMsgBoxes(std::string text, std::queue<MessageBox>& queue);
	void             setMsgBox(MessageBox* m) { currentMsgBox = m; }
	void             onNotify(UIEvent msg);
	void             init();
	void             setGame(Game* g) { game = g; }
	static int       l_loadUISprite(lua_State*L);   // host, path, name                                => void
	static int       l_loadFont(lua_State*L);       // host, font                                      => void
	static int       l_pushToDraw(lua_State*L);     // host, uisprite                                  => void
	static int       l_setToDraw(lua_State*L);      // host, (array of uiSprite equivalent lua tables) => void
	static int       l_clearToDraw(lua_State*L);    // host                                            => void
	static int       l_togglePause(lua_State*L);    // host                                            => void
	static int       l_pause(lua_State*L);          // host                                            => void
private:
	std::vector<UISprite> toDraw;
	void                  updateFPS(float delta); // will be in lua script
	TextRenderer          text_renderer;
	UISpriteRenderer      sprite_renderer;
	void                  renderMsgBox();
	MessageBox*           currentMsgBox = nullptr;
	float                 fps;           // will be in lua script
	//unsigned int          player_HP = 5; // will be in lua script
	Game*                 game;
	void                  registerLuaFunctions();
};

