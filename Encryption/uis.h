#pragma once
#include "directui.h"
#include <string>

class Button : public DirectUI {
protected:
	void Draw(const Render& render) override;
	void OnMsg(Message msg) override;
public:
	D2D_COLOR_F background = {};
	D2D_COLOR_F hover = {};
	D2D_COLOR_F click = {};
	std::wstring text;
	float fontSize = 30;
};