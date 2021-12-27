#include "uis.h"
#include <cassert>
#include <dwrite.h>

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

TextBox::TextBox() : fontSize(30), background(Color(0x8dffffff)), readonly(false), timer(0), cursor(false)
{
	auto p = const_cast<bool*>(&focusAble);
	*p = true;
}

const type_info& TextBox::GetType()
{
	return typeid(TextBox);
}

void TextBox::Draw(const Render& render)
{
	static const auto db = Color(0x00197cff);
	D2D1_RECT_F rect;
	client.ToLeftTopRightBottom(&rect);
	ID2D1SolidColorBrush* bg, * black, * darkblue;
	render.d2d->CreateSolidColorBrush(background, &bg);
	render.d2d->CreateSolidColorBrush(BLACK, &black);
	render.d2d->CreateSolidColorBrush(db, &darkblue);
	render.d2d->FillRectangle(rect, bg);
	if (focus && cursor)
		render.d2d->DrawLine({ client.x + 10,client.y }, { client.x + 10,client.y + fontSize }, darkblue);
	render.d2d->DrawRectangle(rect, black);
	bg->Release();
}

void TextBox::OnMsg(const Message& msg)
{
}

void TextBox::Update(float delta)
{
	static constexpr int degree = 100000;
	timer += static_cast<int>(delta * degree);
	if (timer >= degree / 2)
	{
		timer = 0;
		cursor = !cursor;
	}
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
}

void Label::OnMsg(const Message& msg)
{
}
