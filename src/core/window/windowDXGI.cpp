#include "windowDXGI.h"



LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        ExitProcess(0);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#ifdef __cplusplus
extern "C" {
#endif
void core_window_framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	UNUSED(window);
	glViewport(0, 0, width, height);
}

int core_window_windowInit(struct core_window_Window* window, int width, int height, wchar_t* title, vec4 clearColor) {
	if (window == NULL) {
		printf("WARNING: No core_window_Window object was given to core_window_windowInit. You will not be able to use this window object in the future (you will need to).\n");
	}

	if (width == 0) {
		width = 800;
	}

	if (height == 0) {
		height = 600;
	}

    char *title1 = (char*)"";
	if (wcscmp(title, L"") == 0) {
		title = (wchar_t*)L"Constellation Engine Window";
		title1 = (char*)"Constellation Engine Window";
	}

	window->width = width;
	window->height = height;
	window->shouldClose = 0;

	window->title = title;

    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = TEXT("ConstellationEngineWindow");
    RegisterClassExW(&wc);

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = 0;
    RECT wr = { 0, 0, width, height };
    AdjustWindowRectEx(&wr, dwStyle, FALSE, dwExStyle);

	HWND hWnd = CreateWindowEx(
        dwExStyle, TEXT("ConstellationEngineWindow"), title, dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL, NULL, window->hInstance, NULL);

    ShowWindow(hWnd, SW_SHOWDEFAULT);

    window->window = hWnd;

    window->glfwWin = glfwCreateWindow(width, height, title1, NULL, NULL);
    HWND gl_hWnd = glfwGetWin32Window(window->glfwWin);
    HDC gl_hDC = GetDC(gl_hWnd);

    PIXELFORMATDESCRIPTOR gl_pfd;
    gl_pfd.nSize = sizeof(gl_pfd);
    gl_pfd.nVersion = 1;
    gl_pfd.dwFlags = PFD_SUPPORT_OPENGL;

    int chosenPixelFormat = ChoosePixelFormat(gl_hDC, &gl_pfd);
    HGLRC dummy_hGLRC = glfwGetWGLContext(window->glfwWin);

    wglMakeCurrent(gl_hDC, dummy_hGLRC);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    int contextFlagsGL = 0;
    int contextAttribsGL[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, contextFlagsGL,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC hGLRC = wglCreateContextAttribsARB(gl_hDC, NULL, contextAttribsGL);
    wglMakeCurrent(gl_hDC, hGLRC);
    wglDeleteContext(dummy_hGLRC);

    DXGI_SWAP_CHAIN_DESC1 scd;
    scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferCount = DXGI_MAX_SWAP_CHAIN_BUFFERS;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    //scd.OutputWindow = window->window;
    //scd.Windowed = TRUE;

    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    UINT flags = 0;
    #if defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif
    D3D11CreateDevice(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &window->device,
        NULL,
        &window->devCtx);
    
    IDXGIFactory5 *factory;
    CreateDXGIFactory(__uuidof(IDXGIFactory5), (void**)&factory);
    factory->CreateSwapChainForHwnd(window->device, window->window, &scd, NULL, NULL, &window->swapChain);

    
    PFNWGLDXOPENDEVICENVPROC _wglDXOpenDeviceNV = (PFNWGLDXOPENDEVICENVPROC)wglGetProcAddress("wglDXOpenDeviceNV");
    window->gl_handleD3D = _wglDXOpenDeviceNV(window->device);

    ID3D11Texture2D *dxDepthBuffer;
    CD3D11_TEXTURE2D_DESC textDesc = CD3D11_TEXTURE2D_DESC(
        DXGI_FORMAT_R32G8X24_TYPELESS, width, height, 1, 
        1, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);
    window->device->CreateTexture2D(
        &textDesc,
        NULL,
        &dxDepthBuffer);

    ID3D11DepthStencilView *depthBufferView;
    CD3D11_DEPTH_STENCIL_VIEW_DESC stencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
    window->device->CreateDepthStencilView(
        dxDepthBuffer,
        &stencilViewDesc,
        &depthBufferView);

    GLuint dsvNameGL;
    glGenTextures(1, &dsvNameGL);

    PFNWGLDXREGISTEROBJECTNVPROC wglDXRegisterObjectNV = (PFNWGLDXREGISTEROBJECTNVPROC)wglGetProcAddress("wglDXRegisterObjectNV");
    HANDLE dsvHandleGL = wglDXRegisterObjectNV(window->gl_handleD3D, dxDepthBuffer, dsvNameGL, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dsvNameGL, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &window->rtvNameGL);

	window->clearColor[0] = clearColor[0];
	window->clearColor[1] = clearColor[1];
	window->clearColor[2] = clearColor[2];
	window->clearColor[3] = clearColor[3];

	if (window->glfwWin == NULL) {
		printf("ERROR: Window creation FAILED!\n");

		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window->glfwWin);

	glfwSetFramebufferSizeCallback(window->glfwWin, core_window_framebufferSizeCallback);

	/*if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("ERROR: GLAD initialization FAILED!\n");

		return -1;
	}*/

	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(1);

	return 0;
}

/*int core_window_clear(struct core_window_Window_* window) {
	glClearColor(window->clearColor[0], window->clearColor[1], window->clearColor[2], window->clearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return 0;
}

int core_window_update(struct core_window_Window_* window) {
	if (window == NULL) {
		printf("ERROR: No core_window_Window object was given to the core_window_update function.\n");

		return -1;
	}

	glfwSwapBuffers(window->glfwWin);
	glfwPollEvents();

	if (glfwWindowShouldClose(window->glfwWin)) {
		window->shouldClose = 1;
	}

	return 0;
}*/

#ifdef __cplusplus
};
#endif