#include "app.h"

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	App::Initialize();
	int ret = App::GetInstance()->Run();
	App::Finallize();
	return ret;
}