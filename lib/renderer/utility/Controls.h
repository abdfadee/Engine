#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Listener.h"
#include <functional>

using namespace std;


void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


class Controls {
public:
	inline static GLFWwindow* window;
	inline static vector<std::function<void(GLFWwindow*,float)>> callbacks;
	inline static MouseListener mouseListener;
	inline static ScrollListener scrollListener;
	inline static KeyListener keyListener;


	static void initiliaze(GLFWwindow* window) {
		Controls::window = window;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouseCallback);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyCallback);
	}


	static void addCallback(std::function<void(GLFWwindow*,float)> callback) {
		callbacks.push_back(callback);
	}


	static void processInput(float delta) {
		for (size_t i = 0; i < callbacks.size(); ++i)
			callbacks[i](window,delta);
	}

};



void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	Controls::mouseListener.notifyListeners(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	Controls::scrollListener.notifyListeners(xoffset,yoffset);
}

void keyCallback(GLFWwindow* window, int key , int scancode,int action , int mods) {
	Controls::keyListener.notifyListeners(key,scancode,action,mods);
}