#include "GameObject.h"

#define TEST_SPEED      1.0
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
	glm::vec2 vel = glm::vec2(0.0);
	if (keys & (1 << UP_BIT)) {
		vel += glm::vec2(0, delta*TEST_SPEED);
	}
	if (keys & (1 << DOWN_BIT)) {
		vel += glm::vec2(0, -delta * TEST_SPEED);
	}
	if (keys & (1 << LEFT_BIT)) {
		vel += glm::vec2(-delta * TEST_SPEED, 0);
	}
	if (keys & (1 << RIGHT_BIT)) {
		vel += glm::vec2(delta * TEST_SPEED, 0);
	}
	if(keys == 0) {
		vel += glm::vec2(0.0);
	}
	velocity = vel;
	if (keys & (1 << ZOOM_IN_BIT)) {
		Camera::Instance()->zoom += (TEST_ZOOM_SPEED * delta);
		Camera::Instance()->setPositionClamped(glm::vec2(position.x + velocity.x, position.y + velocity.y));
	}
	else if (keys & (1 << ZOOM_OUT_BIT)) {
		Camera::Instance()->zoom += -(TEST_ZOOM_SPEED * delta);
		Camera::Instance()->setPositionClamped(glm::vec2(position.x + velocity.x, position.y + velocity.y));
	}
	else {
		Camera::Instance()->setPositionClamped(glm::vec2(position.x + velocity.x, position.y + velocity.y));
	}
	lastkeys = keys;
}

void Player::draw(Shader & s, const Camera * camera)
{
	sprite->draw(position, glm::vec3(scale, 1.0), s, camera);
}
