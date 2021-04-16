#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include "Input.h"
#include "Shader.h"
#include "AreaLoader.h"
#include "Camera.h"
GLFWwindow* window;
using namespace glm;
int GLFWInit();

int main(void)
{
	if (__cplusplus == 201703L) std::cout << "C++17\n";
	else if (__cplusplus == 201402L) std::cout << "C++14\n";
	else if (__cplusplus == 201103L) std::cout << "C++11\n";
	else if (__cplusplus == 199711L) std::cout << "C++98\n";
	else std::cout << "pre-standard C++\n";
	if (GLFWInit() < 0) {
		return -1;
	}
	inputInit(window);
	initRendering();
	Shader shader("shaders/background_vert.glsl", "shaders/background_frag.glsl");
	Camera* camera = Camera::Instance();
	Area a;

	if (!AreaLoader::Instance()->loadArea("json", "lvl1_test5.json", a)) {
		std::cout << "failed to load level" << std::endl;
	}

	//b.debugPrint();

	//int testtile = a.getBackGround()->getTileAtPosition(glm::vec2(0, 0), "structure");

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	double delta;
	double last = glfwGetTime();
	do {

		//delta time
		double now = glfwGetTime();
		delta = now - last;
		last = now;
		//update
		//Camera::Instance()->testUpdate(keys, delta, a.getBackGround()->get_base_scale());
		a.update(delta, keys);
		//render
		glClear(GL_COLOR_BUFFER_BIT);
		a.draw(shader, camera);
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			AreaLoader::Instance()->loadArea("json", "lvl1_test5.json", a);
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			AreaLoader::Instance()->loadArea("json", "lvl1_test3.json", a);
		}
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {

		}

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	cleanupRendering();
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

int GLFWInit() {
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 1024, "RPG", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glEnable(GL_BLEND);


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	return 0;
}