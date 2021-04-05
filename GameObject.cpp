#include "GameObject.h"

#define TEST_SPEED      2.0
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
	type = GO_TYPE::PLAYER;
}

Player::~Player()
{
}

void Player::onInteract(GameObject * other)
{
}

void Player::update(float delta, GLuint keys)
{
	//Camera::Instance()->setPositionClamped(position);
	if (keys & (1 << UP_BIT)) {
		velocity = glm::vec2(0, delta*TEST_SPEED);
	}
	if (keys & (1 << DOWN_BIT)) {
		velocity = glm::vec2(0, -delta * TEST_SPEED);
	}
	if (keys & (1 << LEFT_BIT)) {
		velocity = glm::vec2(-delta * TEST_SPEED, 0);
	}
	if (keys & (1 << RIGHT_BIT)) {
		velocity = glm::vec2(delta * TEST_SPEED, 0);
	}
	if(keys == 0) {
		velocity = glm::vec2(0.0);
	}
}

void Player::draw(Shader & s, const Camera * camera)
{
	sprite->draw(position, glm::vec3(scale, 1.0), s, camera);
}
