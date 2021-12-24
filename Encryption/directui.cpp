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
	ID2D1SolidColorBrush* brush;
	ID2D1SolidColorBrush* black;
	IDWriteTextFormat* format;
	render.dwrite->CreateTextFormat(L"", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"zh-cn", &format);
	render.d2d->CreateSolidColorBrush(background, &brush);
	render.d2d->CreateSolidColorBrush({ 0,0,0,1 }, &black);
	assert(brush != nullptr && black != nullptr && format != nullptr);
	render.d2d->FillRectangle(f, brush);
	render.d2d->DrawTextW(text.data(), (UINT32)text.length(), format, f, black);
	render.d2d->DrawRectangle(f, black);
	brush->Release();
	black->Release();
	format->Release();
}

void Button::OnMsg(Message msg)
{
}
