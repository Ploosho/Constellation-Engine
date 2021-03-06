#ifndef MAINH
#define MAINH

// Constellation Engine
#include "core/graphics/graphics.h"
#include "core/input/input.h"
#include "core/window/window.h"

// Thirdparty
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

// Standard
#include <stdio.h>

// Functions
int main();

void processInput(struct core_window_Window* window);

#endif
