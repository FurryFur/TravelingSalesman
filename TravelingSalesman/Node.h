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

#pragma once

#include <functional>

#include <Eigen\Dense>
#include <nanogui\nanogui.h>

class Node : public nanogui::Widget {
public:
	Node(nanogui::Widget* parent);
	~Node() override;

	// Draws the node.
	void draw(NVGcontext* ctx) override;

	using CallbackT = std::function<void(const nanogui::Vector2i& p, int button, bool down, int modifiers)>;
	using DragCallbackT = std::function<void(const nanogui::Vector2i& p, const nanogui::Vector2i& rel, int buttonState, int modifiers)>;

	// Sets the callback function to execute on a mouse click event.
	void setCallback(CallbackT callback);

	// Sets the callback function to execute on a mouse drag event.
	void setDragCallback(DragCallbackT callback);

	// Calls the current mouse click callback function
	bool mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers) override;

	// Calls the current mouse drag callback function.
	bool mouseDragEvent(const nanogui::Vector2i& p, const nanogui::Vector2i& rel, int button, int modifiers) override;

	// Not a virtual function, be careful calling this.
	// Needs to be called on a pointer or reference to type Node.
	// Used to set the position in floating point as well as normal integers.
	// Floating point versions are precalculated here for efficiency
	void setPosition(const nanogui::Vector2i& pos);

	// Not a virtual function, be careful calling this.
	// Needs to be called on a pointer or reference of type Node.
	// Used to set the size in floating point as well as normal integers.
	// Floating point versions are precalculated here for efficiency
	void setSize(const nanogui::Vector2i& size);

	// Returns the position of the node as a floating point type (Vector2d).
	// Used to avoid casting for efficiency.
	Eigen::Vector2d getFloatPos() const;

	// Returns the size of the node as a floating point type (Vector2d).
	// Used to avoid casting for efficiency.
	Eigen::Vector2d getFloatSize() const;

	static const float s_kBorderWidth;
private:
	Eigen::Vector2d m_floatPos;
	Eigen::Vector2d m_floatSize;
	CallbackT m_callback;
	DragCallbackT m_dragCallback;
};