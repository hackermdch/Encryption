#include "app.h"
#include <windowsx.h>
#include <cassert>
#include <dwrite.h>
#include <ShellScalingApi.h>

using namespace std;

static App* app;
static bool initialized = false;
static bool finallized = false;
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
	case WM_IME_CHAR:
	case WM_CHAR:
		if (app->focus != nullptr)
		{
			app->focus->OnMsg(ProccessMessage(msg, wParam, lParam, 0, 0));
			return 0;
		}
		break;
	case WM_KEYDOWN:
		if (app->focus != nullptr)
		{
			app->focus->OnMsg(ProccessMessage(msg, wParam, lParam, 0, 0));
		}
		break;
	case WM_CLOSE:
		app->render = false;
		app->closed = true;
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	{
		const auto x = GET_X_LPARAM(lParam);
		const auto y = GET_Y_LPARAM(lParam);
		if (app->focus != nullptr)
		{
			app->focus->focus = false;
			app->focus->OnMsg(ProccessMessage(FOCUS_EVENT, false, 0, 0, 0));
			ImmAssociateContext(app->hWnd, nullptr);
		}
		for (const auto element : app->elements) {
			if (element->client.Contains(app->TransformPoint(x, y))) {
				if (element->focusAble)
				{
					element->focus = true;
					app->focus = element;
					element->OnMsg(ProccessMessage(FOCUS_EVENT, true, 0, 0, 0));
				}
				element->OnMsg(ProccessMessage(msg, wParam, lParam, x, y));
				break;
			}
		}
		break;
	}
	case WM_LBUTTONUP:
	{
		const auto x = GET_X_LPARAM(lParam);
		const auto y = GET_Y_LPARAM(lParam);
		for (const auto element : app->elements) {
			if (element->client.Contains(app->TransformPoint(x, y))) {
				element->OnMsg(ProccessMessage(msg, wParam, lParam, x, y));
				break;
			}
		}
		break;
	}
	case WM_MOUSEMOVE:
	{
		const auto x = GET_X_LPARAM(lParam);
		const auto y = GET_Y_LPARAM(lParam);
		for (const auto element : app->elements) {
			const bool isInto = element->mouseInto;
			if (element->client.Contains(app->TransformPoint(x, y)) && !isInto) {
				element->mouseInto = true;
				element->OnMsg(ProccessMessage(WM_MOUSEHOVER, wParam, lParam, x, y));
				break;
			}
			if (!element->client.Contains(app->TransformPoint(x, y)) && isInto) {
				element->mouseInto = false;
				element->OnMsg(ProccessMessage(WM_MOUSELEAVE, wParam, lParam, x, y));
				break;
			}
		}
		for (const auto element : app->elements)
		{
			if (element->client.Contains(app->TransformPoint(x, y)) && element->mouseInto) {
				element->OnMsg(ProccessMessage(WM_MOUSEMOVE, wParam, lParam, x, y));
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
	if (!initialized) {
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		WNDCLASSEX wc{};
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
		initialized = true;
	}
}

void App::Finallize()
{
	if (initialized && !finallized) {
		delete app;
		finallized = true;
	}
}

App* App::GetInstance()
{
	return app;
}

App::App() :
	width(default_width),
	height(default_height),
	bufferWidth(default_width),
	bufferHeight(default_height),
	hWnd(CreateWindowExW(0, L"Encrypter", L"¼ÓÃÜÍ¨Ñ¶", WS_OVERLAPPEDWINDOW, 20, 20, default_width, default_height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr)),
	d3d11Device(nullptr),
	closed(false),
	pause(false),
	render(true),
	drawing(false),
	focus(nullptr)
{
	hImc = ImmGetContext(hWnd);
	ImmAssociateContext(hWnd, nullptr);
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
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, fl, 1, D3D11_SDK_VERSION, &dec, &swapChain, &d3d11Device, nullptr, nullptr);
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
	for (const auto item : elements) {
		delete item;
	}
	renderTargetView->Release();
	d2dDeviceContext->Release();
	dwrite->Release();
	swapChain->Release();
	d3d11Device->Release();
}

void App::Render()
{
	static const auto color = Color(0xc7f1ffff);
	drawing = true;
	d2dDeviceContext->BeginDraw();
	d2dDeviceContext->Clear(color);
	struct Render r { d2dDeviceContext, dwrite };
	for (auto element : elements) {
		element->Draw(r);
	}
	d2dDeviceContext->EndDraw();
	swapChain->Present(0, 0);
	drawing = false;
}

void App::Update(float delta)
{
	for (auto e : elements)
	{
		e->Update(delta);
	}
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

bool App::PreTranslateMessage(const MSG* msg) {
	switch (msg->message)
	{
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return false;
	}
	return true;
}

int App::Run()
{
	CreateThread(nullptr, 0, ::timer, nullptr, 0, nullptr);
	timer.Reset();
	ShowWindow(hWnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, nullptr, NULL, NULL))
	{
		if (PreTranslateMessage(&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

void App::ResizeWindow(int width, int height)
{
	SetWindowPos(hWnd, nullptr, 0, 0, width, height, SWP_NOMOVE);
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
	D2D1_CREATION_PROPERTIES cp{};
	cp.debugLevel = D2D1_DEBUG_LEVEL_NONE;
	cp.options = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
	cp.threadingMode = D2D1_THREADING_MODE_MULTI_THREADED;
	D2D1CreateDeviceContext(surface, cp, &d2dDeviceContext);
	d2dDeviceContext->GetTarget((ID2D1Image**)&renderTargetView);
	surface->Release();
	bufferWidth = width;
	bufferHeight = height;
	render = true;
}

void App::SetLocation(int x, int y)
{
	SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE);
}

void App::AddElement(DirectUI* element)
{
	elements.push_back(element);
}

void App::RemoveElement(DirectUI* element)
{
	erase_if(elements, [element](DirectUI* item)->bool { return item == element; });
}

POINT App::TransformPoint(int x, int y)
{
	RECT client;
	GetClientRect(hWnd, &client);
	float sw = (float)bufferWidth / (client.right - client.left);
	float sh = (float)bufferHeight / (client.bottom - client.top);
	return { (int)(sw * x), (int)(sh * y) };
}
