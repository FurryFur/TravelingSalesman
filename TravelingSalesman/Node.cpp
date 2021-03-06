//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A node in the traveling salesman problem.
//                (Callbacks can be set for mouse events)
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include "Node.h"

using namespace nanogui;

const float Node::s_kBorderWidth = 4;

Node::Node(Widget* parent)
	: Widget(parent)
{
	setSize({ 10, 10 });
}

Node::~Node()
{
#ifdef _DEBUG
	std::cout << "Node deleted" << std::endl;
#endif // _DEBUG
}

void Node::draw(NVGcontext* ctx)
{
	nvgSave(ctx);

	// Draw node outline
	nvgBeginPath(ctx);
	nvgRect(ctx, 
	        static_cast<float>(mPos.x()), 
	        static_cast<float>(mPos.y()), 
	        static_cast<float>(mSize.x()), 
	        static_cast<float>(mSize.y()));
	nvgStrokeWidth(ctx, s_kBorderWidth);
	nvgStrokeColor(ctx, nvgRGBA(255, 0, 0, 255));
	nvgStroke(ctx);

	nvgRestore(ctx);

	Widget::draw(ctx);
}

void Node::setCallback(CallbackT callback)
{
	m_callback = callback;
}

void Node::setDragCallback(DragCallbackT callback)
{
	m_dragCallback = callback;
}

bool Node::mouseButtonEvent(const nanogui::Vector2i & p, int button, bool down, int modifiers)
{
	Widget::mouseButtonEvent(p, button, down, modifiers);
	if (m_callback)
		m_callback(p, button, down, modifiers);
	return true; // Prevent further propogation
}

bool Node::mouseDragEvent(const Vector2i& p, const Vector2i& rel, int buttonState, int modifiers)
{
	if (m_dragCallback) {
		m_dragCallback(p, rel, buttonState, modifiers);
		return true;
	}

	return false;
}

void Node::setPosition(const nanogui::Vector2i& pos)
{
	Widget::setPosition(pos);
	m_floatPos = mPos.cast<double>();
}

void Node::setSize(const nanogui::Vector2i& size)
{
	Widget::setSize(size);
	m_floatSize = mSize.cast<double>();
}

Eigen::Vector2d Node::getFloatPos() const
{
	return m_floatPos;
}

Eigen::Vector2d Node::getFloatSize() const
{
	return m_floatSize;
}
