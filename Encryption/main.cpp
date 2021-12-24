#include "app.h"
#include <ShellScalingApi.h>
#include "uis.h"

using namespace std;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	App::Initialize();
	Button* b = new Button();
	b->text = L"test";
	b->background = { 1,1,0,1 };
	b->fontSize = 50;
	b->client = RectangleF{ 500,279,120,45 };
	App::GetInstance()->AddElement(b);
	int ret = App::GetInstance()->Run();
	App::Finallize();
	return ret;
}