#include "CustomButton.h"

using namespace nanogui;

CustomButton::CustomButton(Widget* parent, const std::string& caption, int icon)
	: Button(parent, caption, icon)
{
}

void CustomButton::draw(NVGcontext * ctx)
{
	Button::draw(ctx);

	//nvgBeginPath(ctx);
	//nvgRect(ctx, mPos.x() + 5, mPos.y() + mSize.y() / 2.0f - 5, 10, 10);
	//nvgFillColor(ctx, nvgRGBA(0, 100, 200, 255));
	//nvgFill(ctx);
}
