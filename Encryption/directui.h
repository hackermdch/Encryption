#pragma once
#include <d2d1_2.h>
#include <dwrite.h>

struct RectangleF {
	union
	{
		float x, left;
	};
	union
	{
		float y, top;
	};
	union
	{
		float width, right;
	};
	union
	{
		float height, bottom;
	};
	RectangleF();
	RectangleF(float x, float y, float width, float height);
	bool Contains(int x, int y)const;
	D2D_RECT_F GetRaw() const;
	RectangleF ToLeftTopRightBottom() const;
	RectangleF ToXYWidthHeight() const;
};

struct Message {
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
};

struct Render {
	ID2D1RenderTarget* d2d;
	IDWriteFactory* dwrite;
};

class DirectUI
{
	friend class App;
protected:
	virtual void Draw(const Render& render) = 0;
	virtual void OnMsg(Message msg) = 0;
public:
	struct RectangleF client;
	float opacity = 0;
	bool enable = true;
};

