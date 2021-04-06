#include "Camera.h"
#include <iostream>


#define UP_BIT       0
#define DOWN_BIT     1
#define LEFT_BIT     2
#define RIGHT_BIT    3
#define ZOOM_IN_BIT  4
#define ZOOM_OUT_BIT 5

#define TEST_SPEED 1.0
#define TEST_ZOOM_SPEED 1

#define ORIGINAL_QUAD_WIDTH 2.0
#define ORIGINAL_QUAD_HEIGHT 2.0
Camera* Camera::instance;


void Camera::testUpdate(const GLuint keys, const float delta, glm::vec2 bounds)
{
	bool print = false;
	if (keys & (1 << ZOOM_IN_BIT)) {
		zoom += (TEST_ZOOM_SPEED * delta);
		print = true;
	}
	if (keys & (1 << ZOOM_OUT_BIT)) {
		zoom += -(TEST_ZOOM_SPEED * delta);
		print = true;
	}
	
	glm::vec2 newpos = position;
	if (keys & (1 << UP_BIT)) {
		newpos += glm::vec2(0, delta*TEST_SPEED);
		print = true;
	}
	if (keys & (1 << DOWN_BIT)) {
		newpos += glm::vec2(0, -delta*TEST_SPEED);
		print = true;
	}
	if (keys & (1 << LEFT_BIT)) {
		newpos += glm::vec2(-delta * TEST_SPEED,0);
		print = true;
	}
	if (keys & (1 << RIGHT_BIT)) {
		newpos += glm::vec2(delta * TEST_SPEED,0);
		print = true;
	}
	setPositionClamped(newpos);
	if (print) {
		
		//std::cout << "camera x = " << position.x << std::endl << "camera y = " << position.y <<" zoom= " << zoom << std::endl << std::endl;

	}

}

void Camera::setPositionClamped(glm::vec2 newposition)
{
	newposition *= zoom;
	float bgwidth = zoom * bounds.y; // wrong order?! can't figure out why
	float bgheight = zoom * bounds.x;

	float view_width = 2;
	float view_height = 2;
	

	float xbound = (bgheight - (view_height / 2));
	float ybound = (bgwidth - (view_width / 2));



	if (newposition.x > xbound) {
		position.x = xbound;
	}
	else if (newposition.x < -xbound) {
		position.x = -xbound;
	}
	else {
		position.x = newposition.x;
	}

	if (newposition.y > ybound) {
		position.y = ybound;
	}
	else if (newposition.y < -ybound) {
		position.y = -ybound;
	}
	else {
		position.y = newposition.y;
	}
}
