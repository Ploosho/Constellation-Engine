#ifndef WINDOWDXGIH
#define WINDOWDXGIH

#include <cglm/cglm.h>

// Standard
#include <stdio.h>
#include <string.h>

// Windows
#include <Windows.h>
#include <dxgi1_5.h>
#include <d3d11.h>

// Thirdparty
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32 1
#define GLFW_EXPOSE_NATIVE_WGL 1
#include <GLFW/glfw3native.h>
#define WGL_WGLEXT_PROTOTYPES 1
#include "wglext.h"

// Macros
#define UNUSED(x) (void)(x)
#define DXGI_WINDOW 1
#include "window.h"

// Structs
struct core_window_Window {
	int width;
	int height;
	int shouldClose;

	wchar_t* title;

	HWND window;
    HINSTANCE hInstance;

	vec4 clearColor;
    ID3D11Device *device;
    ID3D11DeviceContext *devCtx;
    IDXGISwapChain1 *swapChain;
    GLFWwindow *glfwWin;
    HANDLE gl_handleD3D;
    GLuint rtvNameGL;
};

#ifdef __cplusplus
extern "C" {
#endif
int core_window_windowInit(struct core_window_Window* window, int width, int height, wchar_t* title, vec4 clearColor);
#ifdef __cplusplus
}
#endif

#endif
