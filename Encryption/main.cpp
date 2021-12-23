#include "app.h"
#include <ShellScalingApi.h>
#include "uis.h"

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	App::Initialize();
	Button* b = new Button();
	b->text = L"test";
	b->background = { 1,1,0,1 };
	b->client = RectangleF{ 500,479,120,60 };
	App::GetInstance()->AddElement(b);
	//App::GetInstance()->ResizeWindow(1920, 1080);
	App::GetInstance()->ResizeBuffer(1920, 1080);
	int ret = App::GetInstance()->Run();
	App::Finallize();
	return ret;
}