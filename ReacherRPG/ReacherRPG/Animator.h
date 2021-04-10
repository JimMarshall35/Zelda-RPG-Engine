#pragma once
#include <map>
#include <string>
#include "Rendering.h"
struct Animation {
	
	Sprite**      frames;
	float         fps;
	bool          shouldloop;
	int           numframes;
};
class Animator
{
public:

	Animator();
	~Animator();
	void freeData();
	void draw(const glm::vec2 pos, const glm::vec2 scale, Shader& s, const Camera* camera);
	void update(float delta);
	void push_animation(std::string anim_name, Sprite** frames, int numframes, float fps, bool loop);
	void start_anim();
	void stop_anim();
	void set_anim(std::string anim_name);
private:
	std::map<std::string, Animation> animations;
	float                            timer;
	Sprite*                          current_frame = nullptr;
	Animation*                       current_animation = nullptr;
	bool                             isanimating;
};

