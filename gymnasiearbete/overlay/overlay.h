#pragma once

#include "../utils/operations/FileOperations.h"
#include "Console/debugConsole.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

class overlay
{
public:
	ImDrawList* drawList;
	HWND window;
	WNDCLASSEXW windowClass;
	
	DXGI_SWAP_CHAIN_DESC swapChain{};
	D3D_FEATURE_LEVEL featureLevels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};
	ID3D11Device* device{ 0 };
	ID3D11DeviceContext* deviceContext{ 0 };
	IDXGISwapChain* loadedSwapChain{ 0 };
	ID3D11RenderTargetView* renderTargetView{ 0 };
	D3D_FEATURE_LEVEL loadedLevel{};
	ID3D11Texture2D* backBuffer{ 0 };

	ImFont* arialFont;

public:
	void registerWindowClass(HINSTANCE instance, WNDPROC WndProc);
	void createWindow();
	void makeFrameIntoClientArea();
	void createDeviceAndSwapchain();
	void initStatics(int nCmdShow); // fonts and textures
	void renderGui(); // goi loop
	bool IsMouseInsideImGuiWindow();
	void destroyWindow(); // to exit program

	void RenderUI();
};

