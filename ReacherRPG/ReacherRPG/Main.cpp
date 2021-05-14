#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include "Input.h"
#include "Shader.h"
#include "Camera.h"
#include "UI.h"
#include "Game.h"
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
	Shader shader("shaders/sprite_vert.glsl", "shaders/sprite_frag.glsl");
	Camera* camera = Camera::Instance();

	Game g;
	g.loadArea("json", "lvl1_test5.json");
	//g.enqueueMsgBoxes("Kick it! You wake up late for school, man you don't want to go. You ask your mom, please? but she still says, no. You missed two classes, and no homework. But your teacher preaches class like you're some kind of jerk. You gotta fight for your right to party.Your pops caught you smoking, and he says, \"No way!\" That hypocrite smokes two packs a day. Man, living at home is such a drag.Now your mom threw away your best porno mag (bust it!). You gotta fight for your right to party. You gotta fight. Don't step out of this house if that's the clothes you're gonna wear. I'll kick you out of my home if you don't cut that hair. Your mom busted in and said, what's that noise?Aw, mom you're just jealous it's the Beastie Boys. You gotta fight for your right to party. You gotta fight for your right to party. Party. Party.");
	
	///////////// leak test /////////////////
	bool leak_test = false;
	std::string files[] = { "lvl1_test5.json",  "lvl1_test3.json" , "interior_test.json" };
	size_t numfiles = 3;
	size_t onfile = 0;
	/////////////////////////////////////////


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
		g.update(delta, keys); 
		//render
		glClear(GL_COLOR_BUFFER_BIT);
		g.draw(shader, camera);

		
		//ui.drawFPS(delta);
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			g.loadArea("json", "lvl1_test5.json");
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			g.loadArea("json", "lvl1_test3.json");
		}
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			g.loadArea("json", "interior_test.json");
		}
		if (leak_test) {
			g.getAreaPtr()->deleteAllGO();
			g.loadArea("json", files[onfile]);
			onfile++;
			if (onfile >= numfiles) {
				onfile = 0;
			}
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
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
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

	GLClearErrors();
	// Ensure we can capture the escape key being pressed below
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	GLPrintErrors("glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);");
	glEnable(GL_BLEND);
	GLPrintErrors("glEnable(GL_BLEND);");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLPrintErrors("glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);");
	return 0;
}
