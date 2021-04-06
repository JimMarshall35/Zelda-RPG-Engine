#pragma once
#include "glm/glm.hpp"
#include "Rendering.h"

enum class GO_TYPE {
	PLAYER,
	DOOR,
	PICKUP,
	ENEMY,
	NONE
};
class GameObject
{
public:
	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 velocity = glm::vec2(0,0);
	glm::vec2 scale = glm::vec2(1.0,1.0);
	GO_TYPE type = GO_TYPE::NONE;
	bool isdrawable = false;
	bool issolidvsbackground = false;
	Sprite* sprite = nullptr;

	virtual void onInteract(GameObject* other) = 0;
	virtual void update(float delta, GLuint keys) = 0;
	virtual void draw(Shader& s, const Camera* camera) {};

};

class Player : public GameObject {
public:
	Player();
	~Player();
	virtual void onInteract(GameObject* other);
	virtual void update(float delta, GLuint keys);
	virtual void draw(Shader& s, const Camera* camera);
private:
	GLuint lastkeys = 0;
	
};

