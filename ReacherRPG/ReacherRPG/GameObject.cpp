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

bool Player::onInteract(GameObject * other)
{
	//std::cout << "COLLISION!" << std::endl;
	return false;
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
	L = Scripting::gameobject_vm.getL();
}

bool ScriptableGameObject::onInteract(GameObject * other)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);              // pushes the lua object corresponding to luaRef
	if (!lua_istable(L, -1)) {
		std::cerr << "reference " << luaRef << "is not a valid table" << std::endl;
	}
	lua_getfield(L, -1, "onInteract");
	if (lua_isfunction(L, -1)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);           // pushes the corresponding lua object 
		lua_pushlightuserdata(L, other);                     // pushes delta 
		if (!Scripting::checkLua(L, lua_pcall(L, 2, 0, 0))) {           // pops keys, delta, self, update
			std::cerr << "lua onInteract call failed" << std::endl;
		}

	}
	else {
		std::cerr << "reference " << luaRef << " member 'onInteract' is not a function" << std::endl;
	}
	lua_settop(L, 0);                                       // clear stack
	return false;
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
		if (!Scripting::checkLua(L, lua_pcall(L, 3, 0, 0))) {           // pops keys, delta, self, update
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
	L = Scripting::gameobject_vm.getL();
	if (Scripting::checkLua(L, luaL_dofile(L,script.c_str()))) {
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
		if (!Scripting::checkLua(L, lua_pcall(L, 1, 0, 0))) {             // pops init function, gameobject
			std::cerr << "lua init call failed, script: " << script << std::endl;
		}
		luaRef = luaL_ref(L, LUA_REGISTRYINDEX);               // pops gameobject table from stack

	}
}

int ScriptableGameObject::l_getPos(lua_State * L)
{	
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	
	float x, y;
	if (go == nullptr) {
		x = 0;
		y = 0;
	}
	else {
		x = go->position.x;
		y = go->position.y;
	}
	
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

int ScriptableGameObject::l_setFloorCollider(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	if (!lua_istable(L, 2)) {
		std::cerr << "argument 2 passed to l_setFloorCollider is not a table" << std::endl;
		return 0;
	}
	float l_offset, r_offset, t_offset, b_offset, px_width, px_height;
	if (!Scripting::gameobject_vm.getTableNumber(L, "left_offset", 2, l_offset)) { return false; }
	if (!Scripting::gameobject_vm.getTableNumber(L, "right_offset", 2, r_offset)) { return false; }
	if (!Scripting::gameobject_vm.getTableNumber(L, "top_offset", 2, t_offset)) { return false; }
	if (!Scripting::gameobject_vm.getTableNumber(L, "bottom_offset", 2, b_offset)) { return false; }
	if (!Scripting::gameobject_vm.getTableNumber(L, "pixelswidth", 2, px_width)) { return false; }
	if (!Scripting::gameobject_vm.getTableNumber(L, "pixelsheight", 2, px_height)) { return false; }

	go->collider.left_offset   = l_offset;
	go->collider.right_offset  = r_offset;
	go->collider.top_offset    = t_offset;
	go->collider.bottom_offset = b_offset;
	go->collider.pixelsheight  = px_height;
	go->collider.pixelswidth   = px_width;

	go->collider.init(go);

	return 0;
}

int ScriptableGameObject::l_setCollidableVsBackground(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	if (!lua_isboolean(L, 2)) {
		std::cerr << "argument 2 must be a boolean l_setCollidableVsBackground" << std::endl;
	}
	bool b = lua_toboolean(L, 2);
	go->issolidvsbackground = b;
 	return 0;
}

int ScriptableGameObject::l_setCollidableVsGameObjects(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	if (!lua_isboolean(L, 2)) {
		std::cerr << "argument 2 must be a boolean l_setCollidableVsGameObjects" << std::endl;
	}
	bool b = lua_toboolean(L, 2);
	go->issolidvsgameobjects = b;
	return 0;
}

int ScriptableGameObject::l_setDrawable(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	if (!lua_isboolean(L, 2)) {
		std::cerr << "argument 2 must be a boolean l_setDrawable" << std::endl;
	}
	bool b = lua_toboolean(L, 2);
	go->isdrawable = b;
	return 0;
}

int ScriptableGameObject::l_setGOType(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	if (!lua_isinteger(L, 2)) {
		std::cerr << "argument 2 not an integer l_setGOType" << std::endl;
		return 0;
	}
	GO_TYPE t = (GO_TYPE)lua_tointeger(L, 2);
	go->type = t;
	return 0;
}

int ScriptableGameObject::l_getGOType(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	lua_pushinteger(L, (int)go->type);
	return 1;
}

int ScriptableGameObject::l_getVelocity(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	lua_pushnumber(L, go->velocity.x);
	lua_pushnumber(L, go->velocity.y);
	return 2;
}

int ScriptableGameObject::l_getPlayerPtr(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	GameObject* p_ptr = go->game->getAreaPtr()->getPlayerPtr();
	if (p_ptr == nullptr) {
		lua_pushinteger(L, 0);
	}
	else {
		lua_pushlightuserdata(L, (void*)p_ptr);
	}
	
	
	return 1;
}

int ScriptableGameObject::l_getEnemiesPtrs(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::vector<GameObject*> gos = go->game->getAreaPtr()->getEnemiesPtrs();
	lua_newtable(L);
	for (size_t i = 0; i < gos.size(); i++) {
		GameObject* enemy = gos[i];
		lua_pushlightuserdata(L, enemy);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

int ScriptableGameObject::l_getLuaObject(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	int luaRef = go->luaRef;
	lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);              // pushes the lua object corresponding to luaRef
	if (!lua_istable(L, -1)) {
		std::cerr << "reference " << luaRef << "is not a valid table" << std::endl;
	}
	return 1;
}

int ScriptableGameObject::l_deleteGO(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	if (go != nullptr) {
		go->game->getAreaPtr()->enqueueForDelete(go);
	}
	
	return 0;
}

int ScriptableGameObject::l_loadArea(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::string folder = luaL_checkstring(L, 2);
	std::string file = luaL_checkstring(L, 3);
	go->game->loadArea(folder, file);
	return 0;
}

int ScriptableGameObject::l_createScriptableGO(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::string script = luaL_checkstring(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_checknumber(L, 4);
	ScriptableGameObject* scriptable = new ScriptableGameObject();
	scriptable->position = glm::vec2(x,y);
	scriptable->setGamePtr(go->game);
	scriptable->init(script);
	go->game->getAreaPtr()->enqueueForCreate(scriptable);
	lua_pushlightuserdata(L,(void*)scriptable);
	return 1;
}

int ScriptableGameObject::l_uiNotifyInt(lua_State * L)
{
	ScriptableGameObject* go = (ScriptableGameObject*)lua_touserdata(L, 1);
	std::string event_type = luaL_checkstring(L, 2);
	int data = luaL_checkinteger (L, 3);
	UIEvent e;
	e.data.setInt(data);
	e.type = event_type;
	e.sendertype = go->type;
	go->game->UINotify(e);

	return 0;
}

inline bool Scripting::LuaVMBase::getLuaTableNumber(lua_State * L, std::string key, int tableIndex, float& out)
{
	lua_pushstring(L, key.c_str());
	int type = lua_gettable(L, 2);
	if (type != LUA_TNUMBER) {
		std::cerr << "key "<< key <<" is not a number." << std::endl;
		return false;
	}
	out = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return true;
}

bool Scripting::checkLua(lua_State * L, int r)
{
	if (r != LUA_OK) {
		std::string error_msg = lua_tostring(L, -1);
		std::cout << error_msg << std::endl;
		return false;
	}
	return true;
}

bool DialogueTrigger::onInteract(GameObject * other)
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
	return false;
}
void Scripting::LuaVMBase::baseInit() {
	L = luaL_newstate();
	luaL_openlibs(L);
}
Scripting::GameObjectVM::GameObjectVM()
{
	
	baseInit();
	if (checkLua(L, luaL_dofile(L, "scripts/engine_defs.lua"))) {}
	registerFunction(ScriptableGameObject::l_getTilesetByName, "getTilesetByName");             // TileSet*             getTilesetByName(host,name) 
	registerFunction(ScriptableGameObject::l_enqueueMsgBoxes, "enqueueMsgBoxes");               // void                 enqueueMsgBoxes(host,msg)

	
	
	registerFunction(ScriptableGameObject::l_setPos, "setPos");                                 // void                 setPos(host,x,y)
	registerFunction(ScriptableGameObject::l_getPos, "getPos");                                 // float x, float y     getPos(host)
	registerFunction(ScriptableGameObject::l_setVelocity, "setVelocity");                       // void                 setVelocity(host,x,y)
	registerFunction(ScriptableGameObject::l_getVelocity, "getVelocity");                       // float x, float y     getVelocity(host)

	registerFunction(ScriptableGameObject::l_setScale, "setScale");                             // void                 setsetScale(host,x,y)
	registerFunction(ScriptableGameObject::l_getScale, "getScale");                             // float x, float y     getScale(host)

	registerFunction(ScriptableGameObject::l_pushAnimation, "pushAnimation");                   // void                 push_animation(host, name, tileset, fps, shouldloop, frames)
	registerFunction(ScriptableGameObject::l_animatorStart, "animatorStart");                   // void                 animatorStart(host)
	registerFunction(ScriptableGameObject::l_animatorStop, "animatorStop");                     // void                 animatorStop(host)
	registerFunction(ScriptableGameObject::l_setAnimation, "setAnimation");                     // void                 setAnimation(host,name)
	registerFunction(ScriptableGameObject::l_getIsAnimating, "getIsAnimating");                 // bool                 getIsAnimating(host)

	registerFunction(ScriptableGameObject::l_setCamClamped, "setCamClamped");                   // void                 setCamClamped(x,y)
	registerFunction(ScriptableGameObject::l_setCamZoom, "setCamZoom");                         // void                 setCamZoom(zoom)
	registerFunction(ScriptableGameObject::l_getCamZoom, "getCamZoom");                         // float                getCamZoom()

	registerFunction(ScriptableGameObject::l_setFloorCollider, "setFloorCollider");             //void                  setFloorCollider(host,collider)
																								/*
																									collider = {
																										left_offset = float,
																										right_offset = float,
																										top_offset = float,
																										bottom_offset = float,
																										pixelswidth = float,
																										pixelsheight = float
																									}
																								*/
	registerFunction(ScriptableGameObject::l_setCollidableVsBackground, "setCollidableBG");     // void                 setCollidableBG(host,value) value = bool
	registerFunction(ScriptableGameObject::l_setCollidableVsGameObjects, "setCollidableGO");    // void                 setCollidableGO(host,value) value = bool
	registerFunction(ScriptableGameObject::l_setDrawable , "setDrawable");                      // void                 setDrawable(host,value)     value = bool

	registerFunction(ScriptableGameObject::l_setGOType, "setGOType");                           // void                 setGOType(host,type) type is GO_TYPE
	registerFunction(ScriptableGameObject::l_getGOType, "getGOType");                           // GO_TYPE              getGOType(host)
	
	registerFunction(ScriptableGameObject::l_getPlayerPtr, "getPlayerPtr");                     // GameObject*          getPlayerPtr(host)
	registerFunction(ScriptableGameObject::l_getEnemiesPtrs, "getEnemiesPtrs");                 // GameObject*[]        getEnemiesPtrs(host)
	
	registerFunction(ScriptableGameObject::l_getLuaObject, "getLuaObject");                     // (lua object)         getLuaObject(host)
	registerFunction(ScriptableGameObject::l_deleteGO, "deleteGO");                             // void                 deleteGO(host)
	registerFunction(ScriptableGameObject::l_createScriptableGO, "createScriptableGO");         // GameObject*          createScriptableGO(host,script,x,y)
	
	registerFunction(ScriptableGameObject::l_loadArea, "loadArea");                             // void                 loadArea(host,folder,file)

	registerFunction(ScriptableGameObject::l_uiNotifyInt, "uiNotifyInt");                       // void                 uiNotifyInt(host, string event_type, int data) 

}


Scripting::GameObjectVM::~GameObjectVM()
{
	freeData();
}

inline void Scripting::LuaVMBase::registerFunction(int(*func)(lua_State *L), std::string func_name)
{
	lua_pushcfunction(L, func);
	lua_setglobal(L, func_name.c_str());
}

void Scripting::LuaVMBase::freeData()
{
	lua_close(L);
}

bool AreaChangeTrigger::onInteract(GameObject * other)
{
	switch (other->type) {
	case GO_TYPE::PLAYER:
		//std::cout << "dialogue box collision" << std::endl;
		if (!spent) {
			game->loadArea(folder,file);
			spent = true;
		}
		return true;
	}
	return false;
}
