#include <iostream>
#include <Windows.h>

#include "../overlay/overlay.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if(console::debug)
		console::allocateConsole();

	// register window class

	// init window

	// fonts and textures

	// render loop

	std::cin.get();
	return 0;
}