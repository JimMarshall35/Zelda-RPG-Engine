#pragma once
#include "glm/glm.hpp"
#include "Rendering.h"
#include "Animator.h"

//#include "Scripting.h"
class Game;
extern "C" {
#include "lua535/include/lua.h"
#include "lua535/include/lauxlib.h"
#include "lua535/include/lualib.h"
}

namespace Scripting {
	class Scripting
	{
	public:
		Scripting();
		~Scripting();
		lua_State* getL() { return L; }

		inline void registerFunction(int(*func)(lua_State*L), std::string func_name);
	private:
		lua_State* L;

	};
	static Scripting s_instance;
}
enum class GO_TYPE {
	PLAYER,
	DOOR,
	PICKUP,
	ENEMY,
	SCENERY,
	ZONE,
	NONE
};
struct Rect {
	float x;
	float y;
	float w;
	float h;
};
class GameObject;
class FloorCollider {
public:
	float top_offset;       // number of pixels ignored from the top down of the sprite
	float bottom_offset;    // number of pixels ignored from the bottom up of the sprite
	float left_offset;      // number of pixels ignored from the left towards the right of the sprite
	float right_offset;     // number of pixels ignored from the right to the left of the sprite
	float pixelswidth;
	float pixelsheight;

	float top;
	float bottom;
	float left;
	float right;

	float width;
	float height;

	float resolutionx;
	float resolutiony;

	void init(GameObject* parent);

};
class GameObject
{
public:
	glm::vec2       position = glm::vec2(0, 0);
	glm::vec2       velocity = glm::vec2(0,0);
	glm::vec2       scale = glm::vec2(1.0,1.0);
	GO_TYPE         type = GO_TYPE::NONE;
	bool            isdrawable = false;
	bool            issolidvsbackground = false;
	bool			issolidvsgameobjects = false;
	bool            isstatic = false;
	FloorCollider   collider;


	virtual void    onInteract(GameObject* other) = 0;
	virtual void    update(float delta, GLuint keys) = 0;
	virtual void    draw(const Shader& s, const Camera* camera) {};
	virtual void    freeData() {};
	void            setGamePtr(Game* g) { game = g; }
protected:
	Game*       game = nullptr;
};

class Player : public GameObject {
public:
	Player();
	~Player();
	virtual void onInteract(GameObject* other);          // when another game object collides with it
	virtual void update(float delta, GLuint keys);
	virtual void draw(const Shader& s, const Camera* camera);
	virtual void freeData() { animator.freeData(); }
	Animator animator;
private:
	enum class DIRECTION {
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};
	GLuint    lastkeys;
	GLuint    lastnewkeys;
	DIRECTION direction = DIRECTION::NONE;
	glm::vec2 lastvelocity = glm::vec2(0,0);
	
};

class StaticSprite : public GameObject{
public:
	StaticSprite() {
		type = GO_TYPE::SCENERY;
		isdrawable = true;
		issolidvsgameobjects = true;
		isstatic = true;
	}
	virtual void onInteract(GameObject* other) {};
	virtual void update(float delta, GLuint keys) {};
	virtual void draw(const Shader& s, const Camera* camera) {
		sprite->draw(position, glm::vec3(scale,1.0f), s, camera);  
	}
	Sprite* sprite;
};
class DialogueTrigger : public GameObject {
public:
	DialogueTrigger() {
		type = GO_TYPE::ZONE;
		isdrawable = false;
		issolidvsgameobjects = false;
		isstatic = true;
	}
	DialogueTrigger(Game* pgame, std::string ptext) {
		game = pgame;
		text = ptext;
		type = GO_TYPE::ZONE;
		isdrawable = false;
		issolidvsgameobjects = false;
		isstatic = true;
	}
	virtual void onInteract(GameObject* other);
	virtual void update(float delta, GLuint keys){}
	void         setGamePtr(Game* pui) { game = pui; }
	void         setString(std::string ptext) { text = ptext; }
private:
	std::string text;
	bool        spent = false;
	

};

bool checkLua(lua_State* L, int r);
class ScriptableGameObject : public GameObject {
public:
	ScriptableGameObject();
	ScriptableGameObject(std::string script) { init(script); }
	~ScriptableGameObject() { freeData(); }
	virtual void    onInteract(GameObject* other);
	virtual void    update(float delta, GLuint keys);
	virtual void    draw(const Shader& s, const Camera* camera) { animator.draw(position, scale, s, camera); }
	virtual void    freeData() { luaL_unref(Scripting::s_instance.getL(), LUA_REGISTRYINDEX, luaRef); };
	void            init(std::string script);
	Animator        animator;
	// lua API
	static int      l_getPos(lua_State* L);          // getPos(host) - returns x, y
	static int      l_setPos(lua_State* L);          // setPos(host,x,y)
	static int      l_enqueueMsgBoxes(lua_State* L); // enqueueMsgBoxes(host,msg)
	static int      l_getTilesetByName(lua_State* L);// getTilesetByName(host,name) - returns ptr to tileset
	static int      l_pushAnimation(lua_State* L);   // push_animation(host, name, tileset, frames, fps, shouldloop)
	static int      l_setVelocity(lua_State* L);     // setVelocity(host,x,y)
	static int      l_getScale(lua_State* L);        // getScale(host) - returns x, y
	static int      l_setScale(lua_State* L);        // setScale(host,x,y)
	static int      l_animatorStart(lua_State* L);   // animatorStart(host);
	static int      l_animatorStop(lua_State* L);    // animatorStop(host);
	static int      l_setAnimation(lua_State* L);    // setAnimation(host,anim_name)
	static int      l_setCamClamped(lua_State* L);   // setCamClamped(x,y)
	static int      l_setCamZoom(lua_State* L);      // setCamZoom(zoom)
	static int      l_getCamZoom(lua_State* L);      // getCamZoom()  - returns a float - the cameras zoom
private:
	int             luaRef = LUA_NOREF;
	lua_State*      L;
};
