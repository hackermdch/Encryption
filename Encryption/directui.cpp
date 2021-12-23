#include "uis.h"
#include <assert.h>

RectangleF::RectangleF() : x(0), y(0), width(0), height(0)
{
}

RectangleF::RectangleF(float x, float y, float width, float height) : x(x), y(y), width(width), height(height)
{
}

RectangleF RectangleF::ToLeftTopRightBottom() const
{
	return RectangleF{ x,y,x + width,y + height };
}

RectangleF RectangleF::ToXYWidthHeight() const
{
	return RectangleF{ x,y,right - x,bottom - y };
}

bool RectangleF::Contains(int x, int y) const
{
	const auto r = this->x + width;
	const auto b = this->y + height;
	return x > this->x && x < r
		&& y > this->y && y < b;
}

D2D_RECT_F RectangleF::GetRaw() const
{
	return D2D_RECT_F{ x,y,width,height };
}

void Button::Draw(const Render& render)
{
	auto&& f = client.ToLeftTopRightBottom().GetRaw();
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
