#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Camera
{
public:
	float zoom;
	glm::vec2 position;
	Camera() {
		position = glm::vec2(0, 0);
		zoom = 3.0;
	}
	~Camera();
	void testUpdate(const GLuint keys, const float delta, glm::vec2 bounds);
	void setPositionClamped(glm::vec2 bounds, glm::vec2 newposition);
};

