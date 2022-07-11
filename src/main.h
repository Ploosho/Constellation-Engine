#ifndef MAINH
#define MAINH

// Constellation Engine
#include "core/graphics/graphics.h"
#ifdef _WIN32
#include "core/window/windowDXGI.h"
#else
#include "core/window/window.h"
#endif
#include "core/input/input.h"

// Thirdparty
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

// Standard
#include <stdio.h>

// Functions
int main();

void processInput(struct core_window_Window* window);

#endif
