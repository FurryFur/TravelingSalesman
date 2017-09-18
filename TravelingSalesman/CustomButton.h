#pragma once

#include <string>

#include <nanogui\nanogui.h>

class CustomButton : public nanogui::Button {
public:
	CustomButton(nanogui::Widget* parent, const std::string& caption = "Untitled", int icon = 0);

	virtual void draw(NVGcontext* ctx) override;
};

