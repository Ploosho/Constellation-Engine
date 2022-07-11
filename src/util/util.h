#ifndef UTILH
#define UTILH

// Standard
#include <stdio.h>
#include <stdlib.h>

// Thirdparty
#include <gl/glew.h>
#include <GLFW/glfw3.h>

const char* util_readFile(char* filePath);
int util_time_getTime();

#endif
