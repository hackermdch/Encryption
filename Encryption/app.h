#pragma once
#include <d2d1.h>
#include <Windows.h>
#include "Timer.h"

class App {
	friend DWORD timer(LPVOID);
public:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void Initialize();
	static void Finallize();
	static App* GetInstance();
	int Run();
	void Resize(int width, int height);
	void SetLocation(int x, int y);
private:
	void Render();
	void Update(float delta);
	App();
	~App();
private:
	const HWND hWnd;
	Timer timer;
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* renderTarget;
	bool closed, pause, render;
	int width, height;
};