#include "app.h"

using namespace std;

static App* app;
constexpr int default_width = 960, default_height = 540;

LRESULT WINAPI App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		app->render = false;
		app->closed = true;
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void App::Initialize()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = L"Encrypter";
	wc.hbrBackground = nullptr;
	wc.hCursor = nullptr;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.lpszMenuName = nullptr;
	RegisterClassEx(&wc);
}

void App::Finallize()
{
	delete app;
}

App* App::GetInstance()
{
	if (app == nullptr) {
		app = new App();
	}
	return app;
}

App::App() :
	width(default_width),
	height(default_height),
	hWnd(CreateWindowExW(0, L"Encrypter", L"¼ÓÃÜÍ¨Ñ¶", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 20, 20, default_width, default_height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr)),
	factory(nullptr),
	renderTarget(nullptr),
	closed(false),
	pause(false),
	render(true)
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	D2D1_RENDER_TARGET_PROPERTIES rtp = {};
	rtp.pixelFormat = D2D1_PIXEL_FORMAT{
		DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE_PREMULTIPLIED
	};
	rtp.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	rtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	D2D1_HWND_RENDER_TARGET_PROPERTIES hwtp = {};
	hwtp.hwnd = hWnd;
	hwtp.pixelSize = D2D1_SIZE_U{ (UINT32)width,(UINT32)height };
	hwtp.presentOptions = D2D1_PRESENT_OPTIONS_IMMEDIATELY;
	factory->CreateHwndRenderTarget(rtp, hwtp, &renderTarget);
	ShowWindow(hWnd, SW_SHOW);
}

App::~App()
{
	renderTarget->Release();
	factory->Release();
}

void App::Render()
{
	renderTarget->BeginDraw();
	renderTarget->Clear({ 0,1,1,1 });
	renderTarget->EndDraw();
}

void App::Update(float delta)
{
}

static DWORD WINAPI timer(LPVOID lpParam)
{
	while (!app->closed) {
		app->timer.Tick();
		if (!app->pause)
		{
			app->Update(app->timer.DeltaTime());
			if (app->render) {
				app->Render();
			}
		}
		else
		{
			Sleep(100);
		}
	}
	return 0;
}

int App::Run()
{
	CreateThread(NULL, 0, ::timer, NULL, 0, NULL);
	timer.Reset();
	MSG msg;
	while (GetMessage(&msg, hWnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void App::Resize(int width, int height)
{
	SetWindowPos(hWnd, 0, 0, 0, width, height, SWP_NOMOVE);
	renderTarget->Resize({ (UINT32)width,(UINT32)height });
	this->width = width;
	this->height = height;
}

void App::SetLocation(int x, int y)
{
	SetWindowPos(hWnd, 0, x, y, 0, 0, SWP_NOSIZE);
}
