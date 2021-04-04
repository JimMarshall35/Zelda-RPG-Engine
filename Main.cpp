#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Input.h"
#include "AreaLoader.h"

GLFWwindow* window;
using namespace glm;
int GLFWInit();

int main(void)
{
	
	if (GLFWInit() < 0) {
		return -1;
	}
	inputInit(window);
	Shader shader("shaders/background_vert.glsl", "shaders/background_frag.glsl");
	Camera camera;
	Area b;
	std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
	if (!AreaLoader::Instance()->loadArea("json","lvl1_test.json", b)) {
		std::cout << "failed to load level" << std::endl;
	}
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	std::cout << "background loading done in " << time_span.count() * 1000 << " ms" << std::endl;
	//b.debugPrint();
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	double delta;
	double last = glfwGetTime();
	do {
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			AreaLoader::Instance()->loadArea("json", "lvl1_test2.json", b);
		}
		//delta time
		double now = glfwGetTime();
		delta = now - last;
		last = now;
		//update
		camera.testUpdate(keys, delta, b.getBackGround()->get_base_scale());
		//render
		glClear(GL_COLOR_BUFFER_BIT);
		b.draw(shader,camera);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

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