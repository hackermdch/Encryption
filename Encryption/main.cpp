#include "app.h"
#include "uis.h"

std::wstring encode(const std::wstring& str);
std::wstring decode(const std::wstring& str);

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	auto&& aa = encode(L"ÎÔ²Ûa");
	auto&& aaa = decode(aa);
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
	TextBox* t = new TextBox();
	t->client = { 50,50,400,350 };
	TextBox* t2 = new TextBox();
	t2->client = { 510,50,400,350 };
	t2->readonly = true;
	Label* l1 = new Label();
	l1->text = L"Ô´ÎÄ±¾:";
	l1->client = { 50,15,120,60 };
	Label* l2 = new Label();
	l2->text = L"Êä³ö:";
	l2->client = { 510,15,120,60 };
	b->click = [t, t2](DirectUI* sender, const Message& msg)->void
	{
		try {
			t2->SetText(encode(t->GetText()));
		}
		catch (std::exception& e)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, string2wstring(std::string(e.what())).data(), L"³ö´íÀ²", MB_ICONERROR | MB_OK);
		}
		catch (...)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, L"Î´Öª´íÎó", L"³ö´íÀ²", MB_ICONERROR | MB_OK);
		}
	};
	b2->click = [t, t2](DirectUI* sender, const Message& msg)->void
	{
		try {
			t2->SetText(decode(t->GetText()));
		}
		catch (std::exception& e)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, string2wstring(std::string(e.what())).data(), L"³ö´íÀ²", MB_ICONERROR | MB_OK);
		}
		catch (...)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, L"Î´Öª´íÎó", L"³ö´íÀ²", MB_ICONERROR | MB_OK);
		}
	};
	App::GetInstance()->AddElement(t2);
	App::GetInstance()->AddElement(t);
	App::GetInstance()->AddElement(b);
	App::GetInstance()->AddElement(b1);
	App::GetInstance()->AddElement(b2);
	App::GetInstance()->AddElement(l1);
	App::GetInstance()->AddElement(l2);
	int ret = App::GetInstance()->Run();
	App::Finallize();
	return ret;
}