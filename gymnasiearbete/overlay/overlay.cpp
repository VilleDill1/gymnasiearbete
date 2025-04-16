#include "overlay.h"
#include <dwmapi.h>
#include <thread>

#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <fstream>
#include <filesystem>

bool UseConfigSystem = false;
bool LoadFilesOnStartUp = false;
bool AutoSave = false;
bool AutoSaveCurrentFile = false;
bool AutoSaveAllFiles = false;
bool SortFiles = false;

std::string extractFilename(const std::string& fullPath) {
	std::size_t lastSlash = fullPath.find_last_of("/\\");
	return (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
}

std::string removeFilename(const std::string& fullPath) {
	std::size_t lastSlash = fullPath.find_last_of("/\\");
	return (lastSlash != std::string::npos) ? fullPath.substr(0, lastSlash + 1) : "";
}


struct File {
	std::string FileName;
	std::string Directory;
	std::string FileContent;
};

void OpenOpenFileDialog(std::string& fileName) {
	OPENFILENAME ofn;
	char szFile[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Open File";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE) {
		fileName = std::string(szFile);
		std::cout << "File selected: " << fileName << std::endl;
	}
	else {
		std::cerr << "No file selected or an error occurred." << std::endl;
	}
}


void CopyTextToClipboard(const std::string& text) {
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();

		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
		if (hMem) {

			char* pMem = (char*)GlobalLock(hMem);
			memcpy(pMem, text.c_str(), text.size() + 1); 
			GlobalUnlock(hMem);

			SetClipboardData(CF_TEXT, hMem);

			CloseClipboard();
		}
		else {
			std::cerr << "Failed to allocate memory for clipboard!" << std::endl;
		}
	}
	else {
		std::cerr << "Failed to open clipboard!" << std::endl;
	}
}

void OpenCreateFileDialog(std::string& fileName) {
	OPENFILENAME ofn;
	char szFile[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Text Files\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Save As";
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&ofn) == TRUE) {
		std::ofstream file(szFile);
		if (file) {
			std::cout << "File created: " << szFile << std::endl;
			fileName = std::string(szFile);
		}
		else {
			std::cerr << "Failed to create file: " << szFile << std::endl;
		}
	}
}

void overlay::RenderUI() {
	static std::vector<File> openFiles;
	static char textBuffer[1024 * 16] = "";
	static int selectedFileIndex = -1;
	static bool showCreateFilePopup = false;

	if (!ImGui::Begin("Text Editor", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::End();
		return;
	}

	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	float leftPanelWidth = min(0.25f * windowSize.x, 170.0f);
	float rightPanelWidth = windowSize.x - leftPanelWidth - ImGui::GetStyle().ItemSpacing.x;

	std::string projName = "Test Text";
	ImVec2 nameSize = ImGui::CalcTextSize(projName.c_str());
	ImGui::BeginChild("Proj Name", ImVec2(leftPanelWidth, nameSize.y * 2.1f), true);
	{
		ImGui::SetCursorPos({ leftPanelWidth / 2 - nameSize.x / 2, nameSize.y / 2.f });
		ImGui::Text(projName.c_str());
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("Buttons", ImVec2(rightPanelWidth, nameSize.y * 2.1f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImGui::SameLine(ImGui::GetWindowWidth() - 71);
		ImGui::SetCursorPosY(7.5f);
		if (ImGui::SmallButton("i")) {
			ImGui::OpenPopup("info");
		}

		if (ImGui::BeginPopup("info")) {
			ImGui::Text("Settings");//
			ImGui::Checkbox("Use Config System", &UseConfigSystem);
			if (UseConfigSystem) {
				ImGui::Checkbox("Load Last Opend Files On Startup", &LoadFilesOnStartUp);
				ImGui::Checkbox("Auto Save", &AutoSave);
				if (AutoSave) {
					ImGui::Checkbox("Auto Save Current FIle", &AutoSaveCurrentFile);
					ImGui::Checkbox("Auto Save All Files", &AutoSaveAllFiles);
				}
				ImGui::Checkbox("Sort Opend Files A-Z", &SortFiles);
			}

			// themes

			ImGui::Text("Infomation");
			ImGui::Text("Build Version: 1.0");
			ImGui::Text("Made By Ville L Te2a Lerums Gymnasium");

			SetWindowLong(window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);
			ImGui::EndPopup();
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 47);
		ImGui::SetCursorPosY(7.5f);
		if (ImGui::SmallButton("-")) {
			ShowWindow(this->window, SW_MINIMIZE);
		}

		ImGui::SameLine();
		ImGui::SetCursorPosY(7.5f);
		if (ImGui::SmallButton("X")) {
			this->destroyWindow();
			exit(0);
		}
	}
	ImGui::EndChild();

	ImGui::BeginChild("FilesPanel", ImVec2(leftPanelWidth, windowSize.y - (nameSize.y * 2.4f)), true);
	ImGui::Text("Open Files");
	ImGui::Separator();

	for (int i = 0; i < openFiles.size(); i++) {
		const auto& file = openFiles[i];
		if (file.Directory.empty() || file.FileName.empty())
			continue;

		if (ImGui::Selectable(file.FileName.c_str(), selectedFileIndex == i)) {
			if (selectedFileIndex != -1) {
				openFiles[selectedFileIndex].FileContent = std::string(textBuffer);
			}
			selectedFileIndex = i;
			std::copy(file.FileContent.begin(), file.FileContent.begin() + min(file.FileContent.size(), sizeof(textBuffer) - 1), textBuffer);
			textBuffer[min(file.FileContent.size(), sizeof(textBuffer) - 1)] = '\0';
		}
	}

	ImGui::Separator();
	static std::string driAndName = "";
	static std::string filename = "";

	if (ImGui::Button("Open File", ImVec2(-1, 25))) {
		OpenOpenFileDialog(driAndName);
		filename = extractFilename(driAndName);

		File newFile = { filename, driAndName, "" };
		openFiles.push_back(newFile);
		openFiles.back().FileContent = fileHandler::loadFile(driAndName);
	}

	if (ImGui::Button("Save File", ImVec2(-1, 25))) {
		if (selectedFileIndex != -1) {
			openFiles[selectedFileIndex].FileContent = std::string(textBuffer);
			fileHandler::saveFile(openFiles[selectedFileIndex].FileName, openFiles[selectedFileIndex].FileContent);
		}
	}

	if (ImGui::Button("Show File Info", ImVec2(-1, 25))) {
		if (selectedFileIndex != -1) {
			ImGui::OpenPopup("File Info");
		}
	}

	if (ImGui::BeginPopup("File Info")) {
		ImGui::Text("General File Info");
		std::string temp = "Full File Path: " + openFiles[selectedFileIndex].Directory;
		ImGui::Text(temp.c_str());
		if (ImGui::Button("Copy Directory")) {
			CopyTextToClipboard(removeFilename(openFiles[selectedFileIndex].Directory));
		}
		SetWindowLong(window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);
		ImGui::EndPopup();
	}

	if (ImGui::Button("Create File", ImVec2(-1, 25))) {
		showCreateFilePopup = true;
	}

	ImGui::EndChild();

	ImGui::SameLine();

	if (selectedFileIndex != -1) {
		ImGui::BeginChild("TextField", ImVec2(rightPanelWidth, windowSize.y - (nameSize.y * 2.4f)), true);
		ImGui::InputTextMultiline("##TextEditor", textBuffer, sizeof(textBuffer), ImVec2(-1, -1));
		ImGui::EndChild();
	}

	static std::string dirAndName = "";
	static std::string fileName = "";
	if (showCreateFilePopup) {
		OpenCreateFileDialog(dirAndName);
		fileName = extractFilename(dirAndName);
		openFiles.push_back({ fileName, dirAndName, "" });
		showCreateFilePopup = false;
	}

	ImGui::End();
}

void overlay::registerWindowClass(HINSTANCE instance, WNDPROC WndProc) {
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.hInstance = instance;
	windowClass.lpfnWndProc = WndProc;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = L"Test Fönster";

	if (!RegisterClassExW(&windowClass)) {
		Logger::log("[ERROR] Failed to Register WNDCLASSEXW");
	}
	else {
		Logger::log("[INFO] WNDCLASSEXW Created");
	}

	return;
}

void overlay::createWindow() {
	HWND windowTemp = nullptr;

	int windowSizeX = GetSystemMetrics(SM_CXSCREEN);
	int windowSizeY = GetSystemMetrics(SM_CYSCREEN);

	windowTemp = CreateWindowExW(NULL, windowClass.lpszClassName, windowClass.lpszClassName,
		WS_POPUP,
		0,
		0,
		windowSizeX,
		windowSizeY,
		0,
		0,
		windowClass.hInstance,
		0
		);

	if (!windowTemp) {
		Logger::log("[ERROR] Window ->", windowTemp);
	}
	else {
		Logger::log("[INFO] Window ->", windowTemp);
	}

	this->window = windowTemp;

	SetLayeredWindowAttributes(this->window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);
}

void overlay::makeFrameIntoClientArea() {

	RECT clientArea{};
	GetClientRect(window, &clientArea);

	RECT windowArea{};
	GetWindowRect(window, &windowArea);

	POINT diff{};
	ClientToScreen(window, &diff);

	const MARGINS margins{
		windowArea.left + (diff.x - windowArea.left),
		windowArea.top + (diff.y - windowArea.top),
		clientArea.right,
		clientArea.bottom
	};

	DwmExtendFrameIntoClientArea(window, &margins);
}

void overlay::createDeviceAndSwapchain() {
	DEVMODE devMode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);

	int frameRate = devMode.dmDisplayFrequency;

	if (!devMode.dmDisplayFrequency) {
		frameRate = 60;
	}

	Logger::log("[INFO] Found DisplayFrequency ->", frameRate);

	swapChain.BufferDesc.RefreshRate.Numerator = frameRate;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.SampleDesc.Count = 1;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 2;
	swapChain.OutputWindow = this->window;
	swapChain.Windowed = TRUE;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChain, &loadedSwapChain, &device, &loadedLevel, &deviceContext) != S_OK) {
		Logger::log("[ERROR] Failed to create Device and swap chain");
	}
	else {
		Logger::log("[INFO] Created Device and swap chain");
	}

	loadedSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	if (!backBuffer) {
		Logger::log("[ERROR] backbuffer ->", backBuffer);
	}
	else {
		Logger::log("[INFO] backbuffer ->", backBuffer);
	}

	if (backBuffer) {
		device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
		backBuffer->Release();
	}
}

void overlay::initStatics(int nCmdShow) {
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	arialFont = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyhbd.ttc", 24.f);

	if (!arialFont) {
		Logger::log("[ERROR] arialFont -> ", arialFont);
	}
	else {
		Logger::log("[INFO] arialFont -> ", arialFont);
	}

	ImGui::StyleColorsDark();

	if (!ImGui_ImplWin32_Init(window)) {
		Logger::log("[ERROR] ImGui_ImplWin32_Init(window)");
	}
	else {
		Logger::log("[INFO] ImGui_ImplWin32_Init(window)");
	}

	if (!ImGui_ImplDX11_Init(device, deviceContext)) {
		Logger::log("[ERROR] ImGui_ImplDX11_Init(device, deviceContext)");
	}
	else {
		Logger::log("[INFO] ImGui_ImplDX11_Init(device, deviceContext)");
	}
}

void overlay::renderGui() {
	bool state = true;
	bool check = false;
	bool exitButton = false;

	while (state) {
		MSG msg;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				state = false;
			}

			if (!state) {
				break;
			}
		}

		if (GetAsyncKeyState(VK_CAPITAL) || exitButton) {
			this->destroyWindow();
			exit(0);
		}

		Sleep(1);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (!ImGui::Begin("Text Editor", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
			ImGui::End();
		}

		if (this->IsMouseInsideImGuiWindow()) {
			SetWindowLong(window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);
		}
		else {
			SetWindowLong(window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW | WS_EX_TRANSPARENT);
		}

		RenderUI();

		ImGui::End;

		ImGui::EndFrame();
		ImGui::Render();

		float color[4]{ 0.f,0.f ,0.f ,0.f };
		deviceContext->OMSetRenderTargets(1, &renderTargetView, 0);
		deviceContext->ClearRenderTargetView(renderTargetView, color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		loadedSwapChain->Present(0, 0);

		if (!check) {
			check = !check;
		}
	}
}

void overlay::destroyWindow() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (loadedSwapChain) {
		loadedSwapChain->Release();
	}
	if (deviceContext) {
		deviceContext->Release();
	}
	if (device) {
		device->Release();
	}
	if (renderTargetView) {
		renderTargetView->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);

	exit(0);
}

inline bool mouseCheckInGui()
{

}

bool overlay::IsMouseInsideImGuiWindow() {
	return ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}