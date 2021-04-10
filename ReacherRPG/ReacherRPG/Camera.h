#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Camera
{
public:
	static Camera* Instance() {
		if (instance == NULL) {
			instance = new Camera();
		}
		return instance;
	}
	float zoom;
	glm::vec2 position;
	
	void testUpdate(const GLuint keys, const float delta, glm::vec2 bounds);
	void setPositionClamped(glm::vec2 newposition);
	void setBounds(glm::vec2 bounds_) { bounds = bounds_; }
private:
	Camera() {
		position = glm::vec2(0, 0);
		zoom = 3.0;
	}
	glm::vec2 bounds;
	static Camera* instance;
};

