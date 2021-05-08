#include "GameObject.h"
#include "Game.h"
#define TEST_SPEED      0.3
#define TEST_ZOOM_SPEED 2
#include "inputdefs.h"
#include "Scripting.h"
Player::Player()
{
	isdrawable = true;
	issolidvsbackground = true;
	issolidvsgameobjects = true;
	type = GO_TYPE::PLAYER;
}

Player::~Player()
{
}

void Player::onInteract(GameObject * other)
{
	//std::cout << "COLLISION!" << std::endl;
}



void Player::update(float delta, GLuint keys)
{
	animator.update(delta);
	
	glm::vec2 vel = glm::vec2(0.0);
	GLuint newkeys = keys & ~(lastkeys);    // any new keys that have been added this frame

	if (newkeys == 0 && keys != lastkeys) { // no new keys have been pressed, but the state is different from the last state
		newkeys = keys;                     // therefore a key that was held last frame has now been released
	}

	if      (newkeys & (1 << UP_BIT))    { direction = DIRECTION::UP; }
	else if (newkeys & (1 << DOWN_BIT))  { direction = DIRECTION::DOWN; }
	else if (newkeys & (1 << LEFT_BIT))  { direction = DIRECTION::LEFT; }
	else if (newkeys & (1 << RIGHT_BIT)) { direction = DIRECTION::RIGHT; }

	if (!(keys & (1 << UP_BIT)) &&
		!(keys & (1 << DOWN_BIT)) &&
		!(keys & (1 << LEFT_BIT)) &&
		!(keys & (1 << RIGHT_BIT))) { direction = DIRECTION::NONE; }

	switch (direction) {
	case DIRECTION::NONE:
		animator.stop_anim();
		vel = glm::vec2(0.0);
		break;
	case DIRECTION::UP:
		animator.set_anim("walk_up");
		animator.start_anim();
		vel += glm::vec2(0, 1);
		break;
	case DIRECTION::DOWN:
		animator.set_anim("walk_down");
		animator.start_anim();
		vel += glm::vec2(0, -1);
		break;
	case DIRECTION::LEFT:
		animator.set_anim("walk_left");
		animator.start_anim();
		vel += glm::vec2(-1, 0);
		break;
	case DIRECTION::RIGHT:
		animator.set_anim("walk_right");
		animator.start_anim();
		vel += glm::vec2(1, 0);
		break;
	}

	velocity = vel * delta * (float)TEST_SPEED;

	if (keys & (1 << ZOOM_IN_BIT)) {
		Camera::Instance()->zoom += (TEST_ZOOM_SPEED * delta);
	}
	else if (keys & (1 << ZOOM_OUT_BIT)) {
		Camera::Instance()->zoom += -(TEST_ZOOM_SPEED * delta);
		if (Camera::Instance()->zoom < 1.0) {
			Camera::Instance()->zoom = 1.0;
		}
	}

	Camera::Instance()->setPositionClamped(glm::vec2(position.x, position.y));

	lastkeys = keys;
}

void Player::draw(const Shader & s, const Camera * camera)
{
	animator.draw(position, scale, s, camera);
}

void FloorCollider::init(GameObject* parent)
{

	top = parent->scale.y - (top_offset / pixelsheight) * 2.0f * parent->scale.y;
	bottom = -parent->scale.y + (bottom_offset / pixelsheight) * 2.0f * parent->scale.y;
	left = -parent->scale.x + (left_offset / pixelswidth) * 2.0f * parent->scale.x;
	right = parent->scale.x - (right_offset / pixelswidth) * 2.0f * parent->scale.x;

	width = right - left;
	height = top - bottom;
	resolutionx = parent->scale.x / pixelswidth;
	resolutiony = parent->scale.y / pixelsheight;
}

ScriptableGameObject::ScriptableGameObject()
{
	L = Scripting::s_instance.getL();
	isdrawable = true;
	issolidvsbackground = true;
	issolidvsgameobjects = true;
	type = GO_TYPE::PLAYER;
}

void ScriptableGameObject::onInteract(GameObject * other)
{

}

void ScriptableGameObject::update(float delta, GLuint keys)
{
	//std::cout << keys << std::endl;
	animator.update(delta);
	lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);              // pushes the lua object corresponding to luaRef
	if (!lua_istable(L, -1)) {
		std::cerr << "reference " << luaRef << "is not a valid table" << std::endl;
	}
	lua_getfield(L, -1, "update");
	if (lua_isfunction(L, -1)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);           // pushes the corresponding lua object 
		lua_pushnumber(L,delta);                             // pushes delta 
		lua_pushinteger(L, keys);                            // pushes keys
		if (!checkLua(L, lua_pcall(L, 3, 0, 0))) {           // pops keys, delta, self, update
			std::cerr << "lua update call failed" << std::endl;
		}

	}
	else {
		std::cerr << "reference " << luaRef << " member 'update' is not a function" << std::endl;
	}
	lua_settop(L, 0);                                       // clear stack
}

void ScriptableGameObject::init(std::string script)
{
	L = Scripting::s_instance.getL();
	if (checkLua(L, luaL_dofile(L,script.c_str()))) {
		lua_getglobal(L, "GameObject");           // pushes gameobject onto stack
		if (!lua_istable(L, -1)) {
			std::cerr << "GameObject is not a table in file " << script << std::endl;
			return;
		}
		lua_pushlightuserdata(L, this);          // pushes this pointer onto the stack
		lua_setfield(L, -2, "host");             // pops this pointer off the stack
		lua_getfield(L, -1, "init");             // push init function onto stack
		if (!lua_isfunction(L, -1)) {
			std::cerr << "GameObject at " << script << " has an init function that isn't a function " << std::endl;
			return;
		}
		lua_getglobal(L, "GameObject");                        // pushes gameobject onto stack
		if (!checkLua(L, lua_pcall(L, 1, 0, 0))) {             // pops init function, gameobject
			std::cerr << "lua init call failed, script: " << script << std::endl;
		}
		luaRef = luaL_ref(L, LUA_REGISTRYINDEX);               // pops gameobject table from stack

	}
}

int ScriptableGameObject::l_getPos(lua_State * L)
{	
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	float x = go->position.x;
	float y = go->position.y;
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

int ScriptableGameObject::l_setPos(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	go->position.x = x;
	go->position.y = y;
	return 0;
}

int ScriptableGameObject::l_enqueueMsgBoxes(lua_State * L)
{
	// enqueueMsgBoxes(host,msg)
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::string msg = std::string(luaL_checkstring(L, 2));
	go->game->enqueueMsgBoxes(msg);
	// no need to pop from stack - new stack per c function call
	return 0;
}

int ScriptableGameObject::l_getTilesetByName(lua_State * L)
{
	// getTilesetByName(host,name) - returns ptr to tileset
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::string name = std::string(luaL_checkstring(L, 2));
	TileSet* tileset = go->game->getAreaPtr()->getTilesetByName(name);
	if (tileset == nullptr) {
		std::cerr << name << " Error - No tileset. l_getTilesetByName" << std::endl;
	}
	lua_pushlightuserdata(L, (void*)tileset);
	return 1;
}

int ScriptableGameObject::l_pushAnimation(lua_State * L)
{
	// push_animation(host, name, tileset, fps, shouldloop, frames)
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::string name = std::string(luaL_checkstring(L, 2));
	TileSet* tileset = (TileSet*)lua_touserdata(L, 3);
	float fps = luaL_checknumber(L, 4);
	bool shouldloop = lua_toboolean(L, 5);
	if (!lua_istable(L, 6)) {
		std::cerr << "lua argument 6 is not a table l_pushAnimation" << std::endl;
	}

	size_t frames_size = lua_rawlen(L, 6);
	Sprite** frames = new Sprite*[frames_size];
	for (size_t i = 0; i < frames_size; i++) {
		int type = lua_rawgeti(L, 6, i+1);        // lua counts from 1... for some reason 
		if (type != LUA_TNUMBER) {
			std::cerr << "frame at index "<< i+1 << std::endl;
		}
		int sindex = lua_tointeger(L, -1);
		lua_pop(L, 1);
		frames[i] = tileset->getSprite(sindex);
	}
	go->animator.push_animation(name, frames, frames_size, fps, shouldloop);
	return 0;
}

int ScriptableGameObject::l_setVelocity(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	go->velocity.x = x;
	go->velocity.y = y;
	return 0;
}

int ScriptableGameObject::l_getScale(lua_State * L)
{
	// getScale(host) - returns x, y
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	float x = go->scale.x;
	float y = go->scale.y;
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
	return 0;
}

int ScriptableGameObject::l_setScale(lua_State * L)
{
	// setScale(host,x,y)
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	go->scale.x = x;
	go->scale.y = y;
	return 0;
}

int ScriptableGameObject::l_animatorStart(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	go->animator.start_anim();
	return 0;
}

int ScriptableGameObject::l_animatorStop(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	go->animator.stop_anim();
	return 0;
}

int ScriptableGameObject::l_setAnimation(lua_State * L)
{
	// setAnimation(host,anim_name)
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::string name = std::string(luaL_checkstring(L, 2));
	go->animator.set_anim(name);
	return 0;
}

int ScriptableGameObject::l_setCamClamped(lua_State * L)
{
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	Camera::Instance()->setPositionClamped(glm::vec2(x, y));
	return 0;
}

int ScriptableGameObject::l_setCamZoom(lua_State * L)
{
	Camera::Instance()->zoom = (float)luaL_checknumber(L, 1);
	return 0;
}

int ScriptableGameObject::l_getCamZoom(lua_State * L)
{
	float zoom = Camera::Instance()->zoom;
	lua_pushnumber(L, zoom);
	return 1;
}

int ScriptableGameObject::l_getIsAnimating(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	bool b = go->animator.get_isanimating();
	lua_pushboolean(L, b);
	return 1;
}

bool checkLua(lua_State * L, int r)
{
	if (r != LUA_OK) {
		std::string error_msg = lua_tostring(L, -1);
		std::cout << error_msg << std::endl;
		return false;
	}
	return true;
}

void DialogueTrigger::onInteract(GameObject * other)
{
	
	switch (other->type) {
	case GO_TYPE::PLAYER:
		//std::cout << "dialogue box collision" << std::endl;
		if (!spent) {
			game->enqueueMsgBoxes(text);
			spent = true;
		}
		break;
	}
}
Scripting::Scripting::Scripting()
{
	L = luaL_newstate();
	luaL_openlibs(L);


	registerFunction(ScriptableGameObject::l_getTilesetByName, "getTilesetByName"); // TileSet*             getTilesetByName(host,name) 
	registerFunction(ScriptableGameObject::l_enqueueMsgBoxes, "enqueueMsgBoxes");   // void                 enqueueMsgBoxes(host,msg)

	
	
	registerFunction(ScriptableGameObject::l_setPos, "setPos");                     // void                 setPos(host,x,y)
	registerFunction(ScriptableGameObject::l_getPos, "getPos");                     // float x, float y     getPos(host)
	registerFunction(ScriptableGameObject::l_setVelocity, "setVelocity");           // void                 setVelocity(host,x,y)

	registerFunction(ScriptableGameObject::l_setScale, "setScale");                 // void                 setsetScale(host,x,y)
	registerFunction(ScriptableGameObject::l_getScale, "getScale");                 // float x, float y     getScale(host)

	registerFunction(ScriptableGameObject::l_pushAnimation, "pushAnimation");       // void                 push_animation(host, name, tileset, fps, shouldloop, frames)
	registerFunction(ScriptableGameObject::l_animatorStart, "animatorStart");       // void                 animatorStart(host)
	registerFunction(ScriptableGameObject::l_animatorStop, "animatorStop");         // void                 animatorStop(host)
	registerFunction(ScriptableGameObject::l_setAnimation, "setAnimation");         // void                 setAnimation(host,name)
	registerFunction(ScriptableGameObject::l_getIsAnimating, "getIsAnimating");     // bool                 getIsAnimating(host)

	registerFunction(ScriptableGameObject::l_setCamClamped, "setCamClamped");       // void                 setCamClamped(x,y)
	registerFunction(ScriptableGameObject::l_setCamZoom, "setCamZoom");             // void                 setCamZoom(zoom)
	registerFunction(ScriptableGameObject::l_getCamZoom, "getCamZoom");             // float                getCamZoom()

}


Scripting::Scripting::~Scripting()
{
	lua_close(L);
}

inline void Scripting::Scripting::registerFunction(int(*func)(lua_State *L), std::string func_name)
{
	lua_pushcfunction(L, func);
	lua_setglobal(L, func_name.c_str());
}
