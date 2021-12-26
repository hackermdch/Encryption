#pragma once
#include <Windows.h>
#include "timer.h"
#include "directui.h"
#include <set>
#include <d3d11.h>

class App final {
	friend DWORD timer(LPVOID);
public:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void Initialize();
	static void Finallize();
	static App* GetInstance();
	int Run();
	void ResizeWindow(int width, int height);
	void ResizeBuffer(int width, int height);
	void SetLocation(int x, int y);
	void AddElement(DirectUI* element);
	void RemoveElement(DirectUI* element);
private:
	void Render();
	void Update(float delta);
	bool PreTranslateMessage(MSG* msg);
	App();
	~App();
	const HWND hWnd{};
	Timer timer;
	ID3D11Device* d3d11Device;
	IDXGISwapChain* swapChain{};
	ID2D1DeviceContext* d2dDeviceContext{};
	IDWriteFactory* dwrite{};
	ID2D1Bitmap1* renderTargetView{};
	bool closed, pause, render;
	int width, height;
	bool drawing;
	std::set<DirectUI*> elements;
};