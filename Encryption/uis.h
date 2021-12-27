#pragma once
#include "directui.h"
#include <string>

class Button : public DirectUI {
protected:
	void Draw(const Render& render) override;
	void OnMsg(const Message& msg) override;
public:
	Button();
public:
	D2D_COLOR_F background;
	D2D_COLOR_F hover_color;
	D2D_COLOR_F click_color;
	std::wstring text;
	float fontSize;
	Event click;
private:
	bool mouse_down;
};

class TextBox : public DirectUI {
protected:
	void Draw(const Render& render) override;
	void OnMsg(const Message& msg) override;
public:
	TextBox();
public:
	D2D_COLOR_F background;
	float fontSize;
};