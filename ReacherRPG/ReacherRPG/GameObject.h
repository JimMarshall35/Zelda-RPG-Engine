#pragma once
#include "glm/glm.hpp"
#include "Rendering.h"
#include "Animator.h"

//#include "Scripting.h"
class Game;
#pragma once
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
	Game*       game = nullptr;

};

bool checkLua(lua_State* L, int r);
class ScriptableGameObject : public GameObject {
public:
	ScriptableGameObject();
	ScriptableGameObject(std::string script) { init(script); }
	~ScriptableGameObject() { freeData(); }
	virtual void    onInteract(GameObject* other);
	virtual void    update(float delta, GLuint keys);
	virtual void    draw(const Shader& s, const Camera* camera) {};
	virtual void    freeData() { luaL_unref(Scripting::s_instance.getL(), LUA_REGISTRYINDEX, luaRef); };
	void            init(std::string script);
	Animator        animator;
private:
	int             luaRef = LUA_NOREF;
	lua_State*      L;
};
