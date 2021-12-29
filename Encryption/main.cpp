#include "app.h"
#include "uis.h"

std::wstring encode(const std::wstring& str);
std::wstring decode(const std::wstring& str);

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	App::Initialize();
	float y = 460;
	Button* b = new Button();
	b->text = L"加密";
	b->client = { 100,y,120,45 };
	Button* b1 = new Button();
	b1->text = L"复制";
	b1->client = { 420,y,120,45 };
	Button* b2 = new Button();
	b2->text = L"解密";
	b2->client = { 740,y,120,45 };
	TextBox* t = new TextBox();
	t->client = { 50,50,400,350 };
	TextBox* t2 = new TextBox();
	t2->client = { 510,50,400,350 };
	t2->readonly = true;
	Label* l1 = new Label();
	l1->text = L"源文本:";
	l1->client = { 50,15,120,60 };
	Label* l2 = new Label();
	l2->text = L"输出:";
	l2->client = { 510,15,120,60 };
	b->click = [t, t2](DirectUI*, const Message&)->void
	{
		try {
			t2->SetText(encode(t->GetText()));
		}
		catch (std::exception& e)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, string2wstring(std::string(e.what())).data(), L"出错啦", MB_ICONERROR | MB_OK);
		}
		catch (...)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, L"未知错误", L"出错啦", MB_ICONERROR | MB_OK);
		}
	};
	b2->click = [t, t2](DirectUI*, const Message&)->void
	{
		try {
			t2->SetText(decode(t->GetText()));
		}
		catch (std::exception& e)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, string2wstring(std::string(e.what())).data(), L"出错啦", MB_ICONERROR | MB_OK);
		}
		catch (...)
		{
			MessageBoxCentered(App::GetInstance()->hWnd, L"未知错误", L"出错啦", MB_ICONERROR | MB_OK);
		}
	};
	b1->click = [t2](DirectUI*, const Message&)->void
	{
		if (OpenClipboard(nullptr)) {
			auto&& a = t2->GetText();
			HANDLE h = GlobalAlloc(GMEM_MOVEABLE, (a.length() + 1) * sizeof(wchar_t));
			h = GlobalLock(h);
			memset(h, 0, (a.length() + 1) * sizeof(wchar_t));
			memcpy(h, a.data(), a.length() * sizeof(wchar_t));
			GlobalUnlock(h);
			SetClipboardData(CF_UNICODETEXT, h);
			CloseClipboard();
			MessageBoxCentered(App::GetInstance()->hWnd, L"已复制", L"提示", MB_ICONINFORMATION | MB_OK);
			return;
		}
		MessageBoxCentered(App::GetInstance()->hWnd, L"无法打开剪贴板，请重试", L"错误", MB_ICONWARNING | MB_OK);
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