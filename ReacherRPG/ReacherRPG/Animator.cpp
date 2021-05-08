#include "Animator.h"



Animator::Animator()
{
}


Animator::~Animator()
{
}

void Animator::freeData()
{
	auto iter = animations.begin();
	while (iter != animations.end()) {
		delete[] iter->second.frames;
		++iter;
	}
	animations.clear();
}

void Animator::draw(const glm::vec2 pos, const glm::vec2 scale, const Shader & s, const Camera * camera)
{
	if (current_frame != nullptr) {
		if (onframe < current_animation->numframes) {
			current_frame->draw(pos, glm::vec3(scale, 1.0), s, camera);
		}
		
	}
	
}

void Animator::update(float delta)
{
	//static int onframe = 0; 
	if (isanimating) {
		
		timer += delta;
		if (timer > 1 / current_animation->fps) {
			timer = 0;
			onframe++;
			if (onframe >= current_animation->numframes && current_animation->shouldloop) {
				onframe = 0;
			}
			else if (onframe >= current_animation->numframes && !current_animation->shouldloop) {
				onframe = current_animation->numframes - 1;
				isanimating = false;
			}
		}
		current_frame = current_animation->frames[onframe];
	}
	else {
		if(current_animation != nullptr)
			current_frame = current_animation->frames[0];
	}
	
}

void Animator::push_animation(std::string anim_name, Sprite ** frames, int numframes, float fps, bool loop)
{
	Animation a;
	a.frames = frames;
	a.numframes = numframes;
	a.fps = fps;
	a.shouldloop = loop;
	animations.insert(std::pair<std::string, Animation>{anim_name, a});
}

void Animator::start_anim()
{
	isanimating = true;
}

void Animator::stop_anim()
{
	isanimating = false;
	if (current_animation != nullptr) {
		current_frame = current_animation->frames[0];
	}
	
	timer = 0;
}


void Animator::set_anim(std::string anim_name)
{
	static std::string last;

	if (animations.find(anim_name) != animations.end()) {
		current_animation = &animations[anim_name];
		if (onframe > current_animation->numframes || anim_name != last) {
			current_frame = current_animation->frames[0];
			onframe = 0;
			last = anim_name;
		}
		
	}
	else {
		std::cout << "Error: animation key " << anim_name << " not found" << std::endl;
	}
	
}
