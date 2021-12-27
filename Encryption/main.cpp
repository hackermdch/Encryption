#include "app.h"
#include <ShellScalingApi.h>
#include "uis.h"

using namespace std;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	App::Initialize();
	float y = 460;
	Button* b = new Button();
	b->text = L"¼ÓÃÜ";
	b->client = { 100,y,120,45 };
	Button* b1 = new Button();
	b1->text = L"¸´ÖÆ";
	b1->client = { 420,y,120,45 };
	Button* b2 = new Button();
	b2->text = L"½âÃÜ";
	b2->client = { 740,y,120,45 };
	App::GetInstance()->AddElement(b);
	App::GetInstance()->AddElement(b1);
	App::GetInstance()->AddElement(b2);
	App::GetInstance()->RemoveElement(b1);
	App::GetInstance()->RemoveElement(b1);
	int ret = App::GetInstance()->Run();
	App::Finallize();
	return ret;
}