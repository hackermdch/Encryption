#include "uis.h"
#include <assert.h>

RectangleF::RectangleF() : x(0), y(0), width(0), height(0)
{
}

RectangleF::RectangleF(float x, float y, float width, float height) : x(x), y(y), width(width), height(height)
{
}

void RectangleF::ToLeftTopRightBottom(void* rect) const
{
	auto p = (RectangleF*)rect;
	p->left = x;
	p->top = y;
	p->right = x + width;
	p->bottom = y + height;
}

void RectangleF::ToXYWidthHeight(void* rect) const
{
	auto p = (RectangleF*)rect;
	p->x = left;
	p->y = top;
	p->width = right - left;
	p->height = bottom - top;
}

bool RectangleF::Contains(int x, int y) const
{
	const auto r = this->x + width;
	const auto b = this->y + height;
	return x > this->x && x < r
		&& y > this->y && y < b;
}

void Button::Draw(const Render& render)
{
	D2D1_RECT_F f;
	client.ToLeftTopRightBottom(&f);
	ID2D1SolidColorBrush* brush, * black;
	IDWriteTextFormat* format; IDWriteTextLayout* layout;
	render.dwrite->CreateTextFormat(L"", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"zh-cn", &format);
	assert(format != nullptr);
	render.dwrite->CreateTextLayout(text.data(), text.length(), format, client.width, client.height, &layout);
	DWRITE_TEXT_METRICS textMetrics;
	layout->GetMetrics(&textMetrics);
	DWRITE_LINE_METRICS* lm = Alloca(DWRITE_LINE_METRICS, textMetrics.lineCount);
	layout->GetLineMetrics(lm, textMetrics.lineCount, &textMetrics.lineCount);
	render.d2d->CreateSolidColorBrush(background, &brush);
	render.d2d->CreateSolidColorBrush({ 0,0,0,1 }, &black);
	assert(brush != nullptr && black != nullptr && layout != nullptr);
	render.d2d->FillRectangle(f, brush);
	IDWriteFontCollection* fc;	IDWriteFontFamily* fm;	IDWriteFont* font;
	format->GetFontCollection(&fc);
	fc->GetFontFamily(0, &fm);
	fm->GetFont(0, &font);
	DWRITE_FONT_METRICS fontMetrics;
	font->GetMetrics(&fontMetrics);
	font->Release();
	fm->Release();
	fc->Release();
	const float line = (double)(fontMetrics.ascent - fontMetrics.descent) * fontSize / fontMetrics.designUnitsPerEm - 0.125 * fontSize;
	float a = lm->baseline - line;
	layout->SetMaxHeight(lm->height);
	render.d2d->DrawTextLayout({ client.x + (client.width - textMetrics.widthIncludingTrailingWhitespace) / 2,client.y - line + (client.height - a) / 2 }, layout, black, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	render.d2d->DrawRectangle(f, black);
	layout->Release();
	brush->Release();
	black->Release();
	format->Release();
}

void Button::OnMsg(Message msg)
{
}
