#include "uis.h"
#include <cassert>
#include <dwrite.h>
#include <sstream>
#include "app.h"

using namespace std;

constexpr static D2D1_COLOR_F BLACK = { 0,0,0,1 };

RectangleF::RectangleF() : x(0), y(0), width(0), height(0)
{
}

RectangleF::RectangleF(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height)
{
}

void RectangleF::ToLeftTopRightBottom(void* rect) const
{
	const auto p = static_cast<RectangleF*>(rect);
	p->left = x;
	p->top = y;
	p->right = x + width;
	p->bottom = y + height;
}

void RectangleF::ToXYWidthHeight(void* rect) const
{
	const auto p = static_cast<RectangleF*>(rect);
	p->x = left;
	p->y = top;
	p->width = right - left;
	p->height = bottom - top;
}

void DirectUI::Update(float delta)
{
}

bool RectangleF::Contains(int x, int y) const
{
	const auto r = this->x + width;
	const auto b = this->y + height;
	return x > this->x && x < r
		&& y > this->y && y < b;
}

bool RectangleF::Contains(const POINT& point) const
{
	const auto r = this->x + width;
	const auto b = this->y + height;
	return point.x > this->x && point.x < r
		&& point.y > this->y && point.y < b;
}

void Button::Draw(const Render& render)
{
	D2D1_RECT_F f;
	client.ToLeftTopRightBottom(&f);
	ID2D1SolidColorBrush* brush, * black;
	IDWriteTextFormat* format; IDWriteTextLayout* layout;
	render.dwrite->CreateTextFormat(L"", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"zh-cn", &format);
	assert(format != nullptr);
	render.dwrite->CreateTextLayout(text.data(), static_cast<UINT32>(text.length()), format, client.width, client.height, &layout);
	DWRITE_TEXT_METRICS text_metrics{};
	layout->GetMetrics(&text_metrics);
	auto* lm = Alloca(DWRITE_LINE_METRICS, text_metrics.lineCount);
	layout->GetLineMetrics(lm, text_metrics.lineCount, &text_metrics.lineCount);
	if (mouse_down) {
		render.d2d->CreateSolidColorBrush(click_color, &brush);
		goto L1;
	}
	if (mouseInto)
	{
		render.d2d->CreateSolidColorBrush(hover_color, &brush);
		goto L1;
	}
	render.d2d->CreateSolidColorBrush(background, &brush);
L1:
	render.d2d->CreateSolidColorBrush(BLACK, &black);
	assert(brush != nullptr && black != nullptr && layout != nullptr);
	IDWriteFontCollection* fc;	IDWriteFontFamily* fm;	IDWriteFont* font;
	format->GetFontCollection(&fc);
	fc->GetFontFamily(0, &fm);
	fm->GetFont(0, &font);
	DWRITE_FONT_METRICS font_metrics{};
	font->GetMetrics(&font_metrics);
	font->Release();
	fm->Release();
	fc->Release();
	const float line = static_cast<float>(font_metrics.ascent - font_metrics.descent) * fontSize / static_cast<float>(font_metrics.designUnitsPerEm) - 0.125f * fontSize;
	const float a = lm->baseline - line;
	layout->SetMaxHeight(lm->height);
	render.d2d->FillRectangle(f, brush);
	render.d2d->DrawTextLayout({ client.x + (client.width - text_metrics.widthIncludingTrailingWhitespace) / 2,client.y - line + (client.height - a) / 2 }, layout, black, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	render.d2d->DrawRectangle(f, black);
	layout->Release();
	brush->Release();
	black->Release();
	format->Release();
}

void Button::OnMsg(const Message& msg)
{
	switch (msg.message)
	{
	case WM_LBUTTONDOWN:
		mouse_down = true;
		break;
	case WM_LBUTTONUP:
		mouse_down = false;
		click(this, msg);
		break;
	}
}

Button::Button() : fontSize(30), background(Color(0xf9d580ff)), hover_color(Color(0x80c9f9ff)), click_color(Color(0x8cf980ff)), mouse_down(false), click(nullptr)
{
}

const type_info& Button::GetType()
{
	return typeid(Button);
}

TextBox::TextBox() : fontSize(24), background(Color(0x8dffffff)), readonly(false), timer(0), cursor(false), row(0), col(0)
{
	auto p = const_cast<bool*>(&focusAble);
	*p = true;
	content.emplace_back();
}

const type_info& TextBox::GetType()
{
	return typeid(TextBox);
}

wstring TextBox::GetText()
{
	while (lock);
	lock = true;
	wstringstream os;
	for (auto& row : content)
	{
		for (auto c : row)
		{
			os.put(c);
		}
	}
	lock = false;
	return os.str();
}

void TextBox::SetText(const std::wstring& text)
{
	while (lock);
	lock = true;
	content.clear();
	content.emplace_back();
	row = col = 0;
	for (int i = 0;; i++) {
		wchar_t c = text[i];
		if (c == 0)break;
		if (c == L'\r')continue;
		if (c == L'\n')
		{
			content[row].insert(content[row].begin() + col, L'\r');
			col++;
			content.emplace(content.begin() + row + 1);
			row++;
			col = 0;
			continue;
		}
		content[row].insert(content[row].begin() + col, c);
		col++;
	}
	lock = false;
}

void TextBox::RenderText(const Render& render, ID2D1Brush* tcolor)
{
	static const auto db = Color(0x00197cff);
	static const auto lineGoup = 5;
	ID2D1SolidColorBrush* darkblue;
	render.d2d->CreateSolidColorBrush(db, &darkblue);
	IDWriteTextFormat* format;
	render.dwrite->CreateTextFormat(L"", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"zh-cn", &format);
	D2D1_RECT_F rect;
	client.ToLeftTopRightBottom(&rect);
	rect.left += 5;
	int r = 0;
	for (auto& row : content)
	{
		wstringstream os;
		D2D1_RECT_F rt(rect);
		rt.top += r * (fontSize + lineGoup);
		for (auto c : row)
		{
			if (c == L'\r')break;
			os.put(c);
		}
		auto&& s = os.str();
		render.d2d->DrawTextW(s.data(), s.length(), format, rt, tcolor);
		r++;
	}
	if (!readonly) {
		if (focus && cursor) {
			auto&& ct = InternalGetText(row, col);
			IDWriteTextLayout* layout;
			render.dwrite->CreateTextLayout(ct.data(), ct.length(), format, client.width, 0, &layout);
			DWRITE_TEXT_METRICS tm;
			layout->GetMetrics(&tm);
			if (tm.lineCount > 1)
			{
				DWRITE_LINE_METRICS* lm = new DWRITE_LINE_METRICS[tm.lineCount];
				layout->GetLineMetrics(lm, tm.lineCount, &tm.lineCount);
				int a = 0;
				for (int i = 0; i < tm.lineCount - 1; i++)
				{
					a += lm[i].length;
				}
				auto&& at = InternalGetText(row, a, col);
				layout->Release();
				render.dwrite->CreateTextLayout(at.data(), at.length(), format, client.width, 0, &layout);
				DWRITE_TEXT_METRICS tm2;
				layout->GetMetrics(&tm2);
				D2D_POINT_2F p1(rect.left + tm2.widthIncludingTrailingWhitespace, rect.top + (row + tm.lineCount - 1) * (fontSize + lineGoup) + lineGoup);
				D2D_POINT_2F p2(p1.x, p1.y + lm->height);
				render.d2d->DrawLine(p1, p2, darkblue);
				layout->Release();
				delete[] lm;
				return;
			}
			D2D_POINT_2F p1(rect.left + tm.widthIncludingTrailingWhitespace, rect.top + row * (fontSize + lineGoup));
			D2D_POINT_2F p2(p1.x, p1.y + tm.height);
			render.d2d->DrawLine(p1, p2, darkblue);
			layout->Release();
		}
	}
	format->Release();
	darkblue->Release();
}

void TextBox::Draw(const Render& render)
{
	D2D1_RECT_F rect;
	client.ToLeftTopRightBottom(&rect);
	ID2D1SolidColorBrush* bg, * black;
	render.d2d->CreateSolidColorBrush(background, &bg);
	render.d2d->CreateSolidColorBrush(BLACK, &black);
	render.d2d->FillRectangle(rect, bg);
	RenderText(render, black);
	render.d2d->DrawRectangle(rect, black);
	bg->Release();
	black->Release();
}

void TextBox::OnMsg(const Message& msg)
{
	switch (msg.message)
	{
	case FOCUS_EVENT:
		if (!readonly && msg.wParam == true) {
			COMPOSITIONFORM cp{};
			cp.dwStyle = CFS_FORCE_POSITION;
			cp.ptCurrentPos = POINT(client.x, client.y - 40);
			ImmAssociateContext(App::GetInstance()->hWnd, App::GetInstance()->hImc);
			ImmSetCompositionWindow(App::GetInstance()->hImc, &cp);
		}
		break;
	case WM_IME_CHAR:
	case WM_CHAR:
	{
		if (!readonly) {
			while (lock);
			lock = true;
			cursor = true;
			timer = 0;
			wchar_t c = (wchar_t)msg.wParam;
			if (c == L'\b') {
				if (col == 0 && row - 1 >= 0) {
					content.erase(content.begin() + row);
					row--;
					col = content[row].size() - 1;
				}
				if (col - 1 >= 0) {
					content[row].erase(content[row].begin() + col - 1);
					col--;
				}
				goto L1;
			}
			content[row].insert(content[row].begin() + col, c);
			col++;
			if (c == L'\r')
			{
				content.emplace(content.begin() + row + 1);
				row++;
				col = 0;
			}
		L1:
			lock = false;
		}
		break;
	}
	case WM_KEYDOWN: {
		switch (msg.wParam)
		{
		case VK_LEFT:
			cursor = true;
			timer = 0;
			if (col == 0 && row - 1 >= 0) {
				row--;
				col = content[row].size();
			}
			if (col - 1 >= 0) {
				col--;
			}
			break;
		case VK_RIGHT:
			cursor = true;
			timer = 0;
			if (col < content[row].size()) {
				col++;
				if (content[row][col - 1] == L'\r')
				{
					if (row + 1 < content.size()) {
						row++;
						col = 0;
					}
				}
			}
			break;
		case 0x56:
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
				if (OpenClipboard(nullptr))
				{
					if (!readonly && IsClipboardFormatAvailable(CF_TEXT)) {
						while (lock);
						lock = true;
						auto data = (const wchar_t*)GetClipboardData(CF_UNICODETEXT);
						for (int i = 0;; i++) {
							wchar_t c = data[i];
							if (c == 0)break;
							if (c == L'\r')continue;
							if (c == L'\n')
							{
								content[row].insert(content[row].begin() + col, L'\r');
								col++;
								content.emplace(content.begin() + row + 1);
								row++;
								col = 0;
								continue;
							}
							content[row].insert(content[row].begin() + col, c);
							col++;
						}
						lock = false;
					}
					CloseClipboard();
				}
			}
			break;
		}
		break;
	}
	}
}

void TextBox::Update(float delta)
{
	if (!readonly) {
		static constexpr int degree = 100000;
		timer += static_cast<int>(delta * degree);
		if (timer >= degree / 2)
		{
			timer = 0;
			cursor = !cursor;
		}
	}
}

wstring TextBox::InternalGetText()
{
	while (lock);
	lock = true;
	wstringstream os;
	for (auto& row : content)
	{
		for (auto c : row)
		{
			os.put(c);
		}
	}
	os.put(L'\n');
	lock = false;
	return os.str();
}

std::wstring TextBox::InternalGetText(int row, int col)
{
	while (lock);
	lock = true;
	wstringstream os;
	auto& r = content[row];
	for (int i = 0; i < col; i++)
	{
		os.put(r[i]);
	}
	lock = false;
	return os.str();
}

std::wstring TextBox::InternalGetText(int row, int start, int end)
{
	while (lock);
	lock = true;
	wstringstream os;
	auto& r = content[row];
	for (int i = start; i < end; i++)
	{
		os.put(r[i]);
	}
	lock = false;
	return os.str();
}

Label::Label() : fontSize(24), color(BLACK)
{
}

const type_info& Label::GetType()
{
	return typeid(Label);
}

void Label::Draw(const Render& render)
{
	D2D1_RECT_F rect;
	client.ToLeftTopRightBottom(&rect);
	ID2D1SolidColorBrush* brush;
	IDWriteTextFormat* format;
	render.dwrite->CreateTextFormat(L"", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"zh-cn", &format);
	render.d2d->CreateSolidColorBrush(color, &brush);
	render.d2d->DrawTextW(text.data(), (UINT32)text.length(), format, rect, brush);
	brush->Release();
	format->Release();
}

void Label::OnMsg(const Message& msg)
{
}
