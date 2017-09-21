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
	// Handle removing nodes on right click
	if (button == GLFW_MOUSE_BUTTON_2 && down) {
		for (size_t i = 0; i < mChildren.size(); ++i) {
			Node* node = dynamic_cast<Node*>(mChildren[i]);
			if (node && node->contains(p)) {
				bool restart = m_pathFinder.stop();

				m_pathFinder.removeNode(node);
				removeChild(static_cast<int>(i));

				if (restart)
					m_pathFinder.calculatePathAsync();

				return true;
			}
		}
	}

	bool handled = Widget::mouseButtonEvent(p, button, down, modifiers);

	// Handle adding nodes on left click
	if (!handled && button == GLFW_MOUSE_BUTTON_1 && down) {
		bool restart = m_pathFinder.stop();

		auto node = new Node(this);
		node->setPosition(p);

		// Setup drag handler for dragging nodes around
		node->setDragCallback([node, this](const Vector2i& p, const Vector2i& rel, int buttonState, int modifiers) {
			if (buttonState & 0x1) { // Not GLFW mouse button apparently (seems to have 1 bit set, going from right to left, for each mouse button pressed)
				bool restart = m_pathFinder.stop();

				node->setPosition(node->position() + rel);

				if (restart)
					m_pathFinder.calculatePathAsync();
			}
		});

		m_pathFinder.addNode(node);

		if (restart)
			m_pathFinder.calculatePathAsync();

		return true;
	}

	return handled;
}

void Canvas::draw(NVGcontext * ctx)
{
	Widget::draw(ctx);

	m_pathFinder.draw(ctx);
}
