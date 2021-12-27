#pragma once
#include <d2d1_2.h>

struct RectangleF {
	union
	{
		float x, left{};
	};
	union
	{
		float y, top{};
	};
	union
	{
		float width, right{};
	};
	union
	{
		float height, bottom{};
	};
	RectangleF();
	RectangleF(float x, float y, float width, float height);
	[[nodiscard]] bool Contains(int x, int y) const;
	[[nodiscard]] bool Contains(POINT point) const;
	void ToLeftTopRightBottom(void* rect) const;
	void ToXYWidthHeight(void* rect) const;
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
protected:
	bool mouseInto = false;
public:
	virtual ~DirectUI() = default;
	RectangleF client;
	float opacity = 0;
	bool enable = true;
};

