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
	bool checkLua(lua_State* L, int r);
	class LuaVMBase {
	public:
		LuaVMBase(){}
		~LuaVMBase() {}
		lua_State* getL() { return L; }

		
		void freeData();
		
	protected:
		inline void registerFunction(int(*func)(lua_State*L), std::string func_name);
		inline bool getLuaTableNumber(lua_State* L, std::string key, int tableIndex, float& out);
		void        baseInit();
		lua_State*  L;
	};
	class GameObjectVM : public LuaVMBase
	{
	public:
		GameObjectVM();
		~GameObjectVM();
		inline bool getTableNumber(lua_State* L, std::string key, int tableIndex, float& out) { return getLuaTableNumber(L, key, tableIndex, out); }

	};
	
	static GameObjectVM gameobject_vm;
}
enum class GO_TYPE {
	NONE = 0,
	PLAYER = 1,
	DOOR = 2,
	PICKUP = 3,
	ENEMY = 4,
	SCENERY = 5,
	ZONE = 6
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


	virtual bool    onInteract(GameObject* other) = 0;
	virtual void    update(float delta, GLuint keys) = 0;
	virtual void    draw(const Shader& s, const Camera* camera) {};
	virtual void    freeData() {};
	void            setGamePtr(Game* g) { game = g; }

protected:
	Game*       game = nullptr;
};

class Player : public GameObject {
	// class not used - replaced by lua script
public:
	Player();
	~Player();
	virtual bool onInteract(GameObject* other);          // when another game object collides with it
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
	virtual bool onInteract(GameObject* other) { 
		if (other->issolidvsgameobjects && issolidvsgameobjects) {
			other->position += -other->velocity;
		}
		return false;
	};
	virtual void update(float delta, GLuint keys) {};
	virtual void draw(const Shader& s, const Camera* camera) {
		if (isdrawable) {
			sprite->draw(position, glm::vec3(scale, 1.0f), s, camera);
		}
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
	virtual bool onInteract(GameObject* other);
	virtual void update(float delta, GLuint keys){}
	void         setGamePtr(Game* pui) { game = pui; }
	void         setString(std::string ptext) { text = ptext; }
private:
	std::string text;
	bool        spent = false;
	

};

class AreaChangeTrigger : public GameObject {
public:
	AreaChangeTrigger() {
		type = GO_TYPE::ZONE;
		isdrawable = false;
		issolidvsgameobjects = false;
		isstatic = true;
	}
	AreaChangeTrigger(Game* pgame, std::string pfolder, std::string pfile) {
		game = pgame;
		folder = pfolder;
		file = pfile;
	}
	virtual bool onInteract(GameObject* other);
	virtual void update(float delta, GLuint keys) {}
	void         setGamePtr(Game* pui) { game = pui; }
private:
	std::string folder;
	std::string file;
	bool        spent = false;


};


class ScriptableGameObject : public GameObject {
public:
	ScriptableGameObject();
	ScriptableGameObject(std::string script) { init(script); }
	~ScriptableGameObject() { freeData(); }
	virtual bool    onInteract(GameObject* other);
	virtual void    update(float delta, GLuint keys);
	virtual void    draw(const Shader& s, const Camera* camera) { animator.draw(position, scale, s, camera); }
	virtual void    freeData() { 
		luaL_unref(Scripting::gameobject_vm.getL(), LUA_REGISTRYINDEX, luaRef);
		animator.freeData();
	};
	void            init(std::string script);
	Animator        animator;
	// lua API
	static int      l_getPos(lua_State* L);                      // getPos(host) - returns x, y
	static int      l_setPos(lua_State* L);                      // setPos(host,x,y)
	static int      l_enqueueMsgBoxes(lua_State* L);             // enqueueMsgBoxes(host,msg)
	static int      l_getTilesetByName(lua_State* L);            // getTilesetByName(host,name) - returns ptr to tileset
	static int      l_pushAnimation(lua_State* L);               // push_animation(host, name, tileset, frames, fps, shouldloop)
	static int      l_setVelocity(lua_State* L);                 // setVelocity(host,x,y)
	static int      l_getScale(lua_State* L);                    // getScale(host) - returns x, y
	static int      l_setScale(lua_State* L);                    // setScale(host,x,y)
	static int      l_animatorStart(lua_State* L);               // animatorStart(host);
	static int      l_animatorStop(lua_State* L);                // animatorStop(host);
	static int      l_setAnimation(lua_State* L);                // setAnimation(host,anim_name)
	static int      l_setCamClamped(lua_State* L);               // setCamClamped(x,y)
	static int      l_setCamZoom(lua_State* L);                  // setCamZoom(zoom)
	static int      l_getCamZoom(lua_State* L);                  // getCamZoom()  - returns a float - the cameras zoom
	static int      l_getIsAnimating(lua_State* L);              // getIsAnimating(host) - returns bool
	static int      l_setFloorCollider(lua_State* L);            // setFloorCollider(host, collider)
	static int      l_setCollidableVsBackground(lua_State* L);   // setCollidableVsBackground(host,value)
	static int      l_setCollidableVsGameObjects(lua_State* L);  // setCollidableVsGameObjects(host,value)
	static int      l_setDrawable(lua_State* L);                 // setDrawable(host,value)
	static int      l_setGOType(lua_State* L);                   // setGOType(host,value) 
	static int      l_getGOType(lua_State* L);                   // getGOType(host) -> returns int
	static int      l_getVelocity(lua_State* L);                 // getVelocity(host) -> returns x, y
	static int      l_getPlayerPtr(lua_State* L);                // getPlayerPtr(host) -> returns ptr to player
	static int      l_getEnemiesPtrs(lua_State* L);              // getEnemiesPtrs(host) -> returns table of GO's of type ENEMY
	static int      l_getLuaObject(lua_State* L);                // getLuaObject(host)
	static int      l_deleteGO(lua_State* L);                    // deleteGO(host)
	static int      l_loadArea(lua_State* L);                    // loadArea(host,folder,file)
	static int      l_createScriptableGO(lua_State* L);          // createScriptableGO(host,script,x,y) -> returns ptr to created GO
	
	static int      l_uiNotifyInt(lua_State* L);                 // uiNotifyInt(host, string event_type, int data) 
private:
	int             luaRef = LUA_NOREF;
	lua_State*      L;
};
