#include <iostream>
#include <Windows.h>

#include "../overlay/overlay.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return 0;
	}
	switch (msg) {
	case WM_SYSCOMMAND: {
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if(console::debug)
		console::allocateConsole();

	overlay overlay;

	// register window class
	overlay.registerWindowClass(hInstance, Wndproc);

	// init window
	overlay.createWindow();

	// window stuff
	overlay.makeFrameIntoClientArea();

	// Device and swapchain
	overlay.createDeviceAndSwapchain();

	// fonts and textures
	overlay.initStatics(nShowCmd);

	Sleep(1000);
	//console::minimizeConsole();

	// render loop
	overlay.renderGui();

	exit(0);
	return 0;
}