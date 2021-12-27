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
	b->text = L"����";
	b->client = { 100,y,120,45 };
	Button* b1 = new Button();
	b1->text = L"����";
	b1->client = { 420,y,120,45 };
	Button* b2 = new Button();
	b2->text = L"����";
	b2->client = { 740,y,120,45 };
	TextBox* t = new TextBox();
	t->client = { 50,50,400,350 };
	TextBox* t2 = new TextBox();
	t2->client = { 510,50,400,350 };
	Label* l1 = new Label();
	l1->text = L"Դ�ı�:";
	l1->client = { 50,15,120,60 };
	Label* l2 = new Label();
	l2->text = L"���:";
	l2->client = { 510,15,120,60 };
	App::GetInstance()->AddElement(t);
	App::GetInstance()->AddElement(t2);
	App::GetInstance()->AddElement(b);
	App::GetInstance()->AddElement(b1);
	App::GetInstance()->AddElement(b2);
	App::GetInstance()->AddElement(l1);
	App::GetInstance()->AddElement(l2);
	int ret = App::GetInstance()->Run();
	App::Finallize();
	return ret;
}