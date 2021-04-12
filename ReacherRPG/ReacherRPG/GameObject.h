#pragma once
#include "glm/glm.hpp"
#include "Rendering.h"
#include "Animator.h"

enum class GO_TYPE {
	PLAYER,
	DOOR,
	PICKUP,
	ENEMY,
	SCENERY,
	NONE
};
struct Rect {
	float x;
	float y;
	float w;
	float h;
};
struct FloorCollider {
	float resolution;
	float top_offset;       // number of pixels ignored from the top down of the sprite
	float bottom_offset;    // number of pixels ignored from the bottom up of the sprite
	float left_offset;      // number of pixels ignored from the left towards the right of the sprite
	float right_offset;     // number of pixels ignored from the right to the left of the sprite
	float pixelswidth;
	float pixelsheight;
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
	FloorCollider   collider;
	unsigned int    renderlayer;

	virtual void onInteract(GameObject* other) = 0;
	virtual void update(float delta, GLuint keys) = 0;
	virtual void draw(Shader& s, const Camera* camera) {};
	virtual void freeData() {};
};

class Player : public GameObject {
public:
	Player();
	~Player();
	virtual void onInteract(GameObject* other);          // when another game object collides with it
	virtual void update(float delta, GLuint keys);
	virtual void draw(Shader& s, const Camera* camera);
	virtual void freeData() { animator.freeData(); }
	Animator animator;
private:
	GLuint lastkeys = 0;
	
};

class StaticSprite : public GameObject{
public:
	StaticSprite() {
		type = GO_TYPE::SCENERY;
		isdrawable = true;
		issolidvsgameobjects = true;
	}
	virtual void onInteract(GameObject* other) {};
	virtual void update(float delta, GLuint keys) {};
	virtual void draw(Shader& s, const Camera* camera) { 
		sprite->draw(position, glm::vec3(scale,1.0f), s, camera);  
	}
	Sprite* sprite;
};