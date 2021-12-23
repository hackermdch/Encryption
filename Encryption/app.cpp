#include "app.h"
#include <windowsx.h>
#include <assert.h>

using namespace std;

static App* app;
constexpr int default_width = 960, default_height = 540;

__forceinline static Message ProccessMessage(UINT msg, WPARAM wParam, LPARAM lParam, int x, int y) {
	Message msg1 = {};
	msg1.pt.x = x;
	msg1.pt.y = y;
	msg1.message = msg;
	msg1.wParam = wParam;
	msg1.lParam = lParam;
	return msg1;
}

LRESULT WINAPI App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		app->render = false;
		app->closed = true;
		break;
	case WM_LBUTTONDOWN:
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		for (auto element : app->elements) {

			if (element->client.Contains(x, y)) {
				element->OnMsg(ProccessMessage(msg, wParam, lParam, x, y));
				break;
			}
		}
		break;
	}
	case WM_LBUTTONUP:
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		for (auto element : app->elements) {

			if (element->client.Contains(x, y)) {
				element->OnMsg(ProccessMessage(msg, wParam, lParam, x, y));
				break;
			}
		}
		break;
	}
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
	app = new App();
}

void App::Finallize()
{
	delete app;
}

App* App::GetInstance()
{
	return app;
}

App::App() :
	width(default_width),
	height(default_height),
	hWnd(CreateWindowExW(0, L"Encrypter", L"¼ÓÃÜÍ¨Ñ¶", WS_SIZEBOX | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 20, 20, default_width, default_height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr)),
	d3d11Device(nullptr),
	closed(false),
	pause(false),
	render(true),
	drawing(false)
{
	D3D_FEATURE_LEVEL fl[]{ D3D_FEATURE_LEVEL_11_1 };
	DXGI_SWAP_CHAIN_DESC dec = {};
	DXGI_MODE_DESC mode = {};
	mode.Width = width;
	mode.Height = height;
	mode.RefreshRate.Denominator = 0;
	mode.RefreshRate.Numerator = 0;
	mode.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	dec.BufferCount = 1;
	dec.BufferDesc = mode;
	dec.Windowed = true;
	dec.SampleDesc.Count = 1;
	dec.SampleDesc.Quality = 0;
	dec.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dec.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dec.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	dec.OutputWindow = hWnd;
	auto res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, fl, 1, D3D11_SDK_VERSION, &dec, &swapChain, &d3d11Device, nullptr, nullptr);
	IDXGISurface* surface;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
	assert(surface != nullptr);
	D2D1_CREATION_PROPERTIES cp = {};
	cp.debugLevel = D2D1_DEBUG_LEVEL_NONE;
	cp.threadingMode = D2D1_THREADING_MODE_MULTI_THREADED;
	cp.options = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
	D2D1CreateDeviceContext(surface, cp, &d2dDeviceContext);
	d2dDeviceContext->GetTarget((ID2D1Image**)&renderTargetView);
	IDXGIDevice* dxgi;
	IDXGIAdapter* ad;
	IDXGIFactory* f;
	d3d11Device->QueryInterface(&dxgi);
	assert(dxgi != nullptr);
	dxgi->GetParent(IID_PPV_ARGS(&ad));
	ad->GetParent(IID_PPV_ARGS(&f));
	f->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	f->Release();
	ad->Release();
	dxgi->Release();
	surface->Release();
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(dwrite), (IUnknown**)&dwrite);
}

App::~App()
{
	for (auto item : elements) {
		delete item;
	}
	d3d11Device->Release();
	d2dDeviceContext->Release();
}

void App::Render()
{
	drawing = true;
	d2dDeviceContext->BeginDraw();
	d2dDeviceContext->Clear({ 0,1,1,1 });
	struct Render r { d2dDeviceContext, dwrite };
	for (auto element : elements) {
		element->Draw(r);
	}
	d2dDeviceContext->EndDraw();
	d2dDeviceContext->Flush();
	swapChain->Present(0, 0);
	drawing = false;
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
	ShowWindow(hWnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, hWnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void App::ResizeWindow(int width, int height)
{
	SetWindowPos(hWnd, 0, 0, 0, width, height, SWP_NOMOVE);
	this->width = width;
	this->height = height;
}

void App::ResizeBuffer(int width, int height) {
	render = false;
	while (drawing);
	d2dDeviceContext->SetTarget(nullptr);
	renderTargetView->Release();
	d2dDeviceContext->Release();
	swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	IDXGISurface* surface;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
	assert(surface != nullptr);
	D2D1_CREATION_PROPERTIES cp = {};
	cp.debugLevel = D2D1_DEBUG_LEVEL_NONE;
	cp.options = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
	cp.threadingMode = D2D1_THREADING_MODE_MULTI_THREADED;
	D2D1CreateDeviceContext(surface, cp, &d2dDeviceContext);
	d2dDeviceContext->GetTarget((ID2D1Image**)&renderTargetView);
	surface->Release();
	render = true;
}

void App::SetLocation(int x, int y)
{
	SetWindowPos(hWnd, 0, x, y, 0, 0, SWP_NOSIZE);
}

void App::AddElement(DirectUI* element)
{
	elements.insert(element);
}

void App::RemoveElement(DirectUI* element)
{
	if (elements.contains(element)) {
		elements.erase(element);
	}
}
