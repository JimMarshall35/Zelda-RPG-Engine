#include "GameObject.h"

#define TEST_SPEED      0.3
#define TEST_ZOOM_SPEED 1
#define UP_BIT       0
#define DOWN_BIT     1
#define LEFT_BIT     2
#define RIGHT_BIT    3
#define ZOOM_IN_BIT  4
#define ZOOM_OUT_BIT 5
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
	if (keys & (1 << UP_BIT)) {
		animator.set_anim("walk_up");
		animator.start_anim();
		vel += glm::vec2(0, 1);
	}
	if (keys & (1 << DOWN_BIT)) {
		animator.set_anim("walk_down");
		animator.start_anim();
		vel += glm::vec2(0, -1);
	}
	if (keys & (1 << LEFT_BIT)) {
		animator.set_anim("walk_left");
		animator.start_anim();
		vel += glm::vec2(-1, 0);
	}
	if (keys & (1 << RIGHT_BIT)) {
		animator.set_anim("walk_right");
		animator.start_anim();
		vel += glm::vec2(1, 0);
	}
	if(!(keys & (1<<UP_BIT)) &&
	   !(keys & (1 << DOWN_BIT)) &&
	   !(keys & (1 << LEFT_BIT)) &&
	   !(keys & (1 << RIGHT_BIT))) {
		animator.stop_anim();
		vel = glm::vec2(0.0);
	}
	if (glm::length(vel) > 0) {
		vel = glm::normalize(vel) * delta * (float)TEST_SPEED;
		
	}
	velocity = vel;
	if (keys & (1 << ZOOM_IN_BIT)) {
		Camera::Instance()->zoom += (TEST_ZOOM_SPEED * delta);
		Camera::Instance()->setPositionClamped(glm::vec2(position.x + velocity.x, position.y + velocity.y));
	}
	else if (keys & (1 << ZOOM_OUT_BIT)) {
		Camera::Instance()->zoom += -(TEST_ZOOM_SPEED * delta);
		if (Camera::Instance()->zoom < 1.0) {
			Camera::Instance()->zoom = 1.0;
		}
		Camera::Instance()->setPositionClamped(glm::vec2(position.x + velocity.x, position.y + velocity.y));
	}
	else {
		Camera::Instance()->setPositionClamped(glm::vec2(position.x + velocity.x, position.y + velocity.y));
	}
	lastkeys = keys;
}

void Player::draw(Shader & s, const Camera * camera)
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

ScriptableGameObject::ScriptableGameObject(std::string script)
{
	L = luaL_newstate();
	luaL_openlibs(L);
}

void ScriptableGameObject::onInteract(GameObject * other)
{
}

void ScriptableGameObject::update(float delta, GLuint keys)
{
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
