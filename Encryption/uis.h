#pragma once
#include "directui.h"
#include <string>

class Button : public DirectUI
{
protected:
	void Draw(const Render& render) override;
	void OnMsg(const Message& msg) override;
public:
	Button();
	const type_info& GetType() override;
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

class TextBox : public DirectUI
{
private:
	void RenderText(const Render& render);
protected:
	void Draw(const Render& render) override;
	void OnMsg(const Message& msg) override;
	void Update(float) override;
public:
	TextBox();
	const type_info& GetType() override;
	std::wstring GetText();
public:
	D2D_COLOR_F background;
	float fontSize;
	bool readonly;
private:
	int timer;
	bool cursor;
	std::vector<std::vector<wchar_t>> content;
};

class Label : public DirectUI
{
protected:
	void Draw(const Render& render) override;
	void OnMsg(const Message& msg) override;
public:
	Label();
	const type_info& GetType() override;
public:
	D2D1_COLOR_F color;
	float fontSize;
	std::wstring text;
};