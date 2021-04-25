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
	std::cout << "COLLISION!" << std::endl;
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
	/*
	top1 = go1->position.y + go1->scale.y - (go1->collider.top_offset / go1->collider.pixelsheight) * 2.0f * go1->scale.y;
	bottom1 = go1->position.y - go1->scale.y + (go1->collider.bottom_offset / go1->collider.pixelsheight) * 2.0f * go1->scale.y;
	left1 = go1->position.x - go1->scale.x + (go1->collider.left_offset / go1->collider.pixelswidth) * 2.0f * go1->scale.x;
	right1 = go1->position.x + go1->scale.x - (go1->collider.right_offset / go1->collider.pixelswidth) * 2.0f * go1->scale.x;
	*/
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

}

void ScriptableGameObject::onInteract(GameObject * other)
{
}

void ScriptableGameObject::update(float delta, GLuint keys)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);              // pushes the corresponding lua object to luaRef
	if (!lua_istable(L, -1)) {
		std::cerr << "reference " << luaRef << "is not a valid table" << std::endl;
	}
	lua_getfield(L, -1, "update");                           // pushes update 
	if (lua_isfunction(L, -1)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);           // pushes the corresponding lua object 
		lua_pushnumber(L,delta);                             // pushes delta 
		lua_pushinteger(L, keys);                            // pushes keys
		if (!checkLua(L, lua_pcall(L, 3, 0, 0))) {           // pops keys, delta, self, update
			std::cerr << "lua update call failed" << std::endl;
		}

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
			std::cerr << "game object at " << script << " has an init function that isn't a function " << std::endl;
			return;
		}
		lua_getglobal(L, "GameObject");                        // pushes gameobject onto stack
		if (!checkLua(L, lua_pcall(L, 1, 0, 0))) {             // pops init function, gameobject
			std::cerr << "lua init call failed" << std::endl;
		}
		luaRef = luaL_ref(L, LUA_REGISTRYINDEX);               // pops gameobject table from stack

	}
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
}


Scripting::Scripting::~Scripting()
{
	lua_close(L);
}
