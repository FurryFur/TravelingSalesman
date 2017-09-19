#include "Canvas.h"
#include "Node.h"
#include "PathFinder.h"

using namespace nanogui;

Canvas::Canvas(Widget* parent, PathFinder& pathFinder)
	: Widget(parent)
	, m_pathFinder{ pathFinder }
{
}


Canvas::~Canvas()
{
}

bool Canvas::mouseButtonEvent(const nanogui::Vector2i & p, int button, bool down, int modifiers)
{
	if (button == GLFW_MOUSE_BUTTON_2 && down) {
		for (size_t i = 0; i < mChildren.size(); ++i) {
			Node* node = dynamic_cast<Node*>(mChildren[i]);
			if (node && node->contains(p)) {
				m_pathFinder.removeNode(node);
				removeChild(i);
				return true;
			}
		}
	}

	bool handled = Widget::mouseButtonEvent(p, button, down, modifiers);

	if (!handled && button == GLFW_MOUSE_BUTTON_1 && down) {
		auto node = new Node(this);
		node->setPosition(p);
		m_pathFinder.addNode(node);
		return true;
	}

	return handled;
}
