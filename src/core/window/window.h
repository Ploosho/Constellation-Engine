#ifndef WINDOWH
#define WINDOWH

// Thirdparty
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

// Standard
#include <stdio.h>
#include <string.h>

// Macros
#define UNUSED(x) (void)(x)

// Structs
struct core_window_Window {
	int width;
	int height;
	int shouldClose;

	char* title;

	GLFWwindow* window;

	vec4 clearColor;
};

// Functions
int core_window_windowInit(struct core_window_Window* window, int width, int height, char* title, vec4 clearColor);
int core_window_clear(struct core_window_Window* window);
int core_window_update(struct core_window_Window* window);
int core_window_cleanup();

void core_window_framebufferSizeCallback(GLFWwindow* window, int width, int height);

#endif
