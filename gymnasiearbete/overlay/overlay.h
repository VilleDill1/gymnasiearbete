#pragma once

#include "Console/debugConsole.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

class overlay
{
public:
	ImDrawList* drawList;


private:
	void registerWindowClass();
	void createWindow();
	void initStatics(); // fonts and textures
	void renderGui(); // goi loop
	void destroyWindow(); // to exit program
};

