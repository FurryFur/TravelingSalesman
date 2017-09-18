#include <algorithm>

#include "Node.h"
#include "Grid.h"
#include "NavPainter.h"

using namespace nanogui;

const float Node::s_kBorderWidth = 4;

Node::Node(Window* window, NavPainter& navPainter, size_t row, size_t col)
	: Widget(window)
	, m_navPainter{ navPainter }
	, m_row{ row }
	, m_col{ col }
	, m_obstructed{ false }
{
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

	// Draw node connections
	for (Node* node : m_connections) {
		nvgStrokeWidth(ctx, 2.0f);
		nvgStrokeColor(ctx, nvgRGBA(0, 255, 0, 100));
		nvgBeginPath(ctx);
		// Move to center of node
		float startX = mPos.x() + mSize.x() / 2.0f;
		float startY = mPos.y() + mSize.y() / 2.0f;
		nvgMoveTo(ctx, startX, startY);
		// Draw to center of connected node
		float endX = node->mPos.x() + node->mSize.x() / 2.0f;
		float endY = node->mPos.y() + node->mSize.y() / 2.0f;
		nvgLineTo(ctx, endX, endY);
		nvgStroke(ctx);
	}

	// Draw obstructions
	if (m_obstructed) {
		nvgBeginPath(ctx);
		nvgRect(ctx, 
		        static_cast<float>(mPos.x() + s_kBorderWidth / 2),
		        static_cast<float>(mPos.y() + s_kBorderWidth / 2),
		        static_cast<float>(mSize.x() - s_kBorderWidth),
		        static_cast<float>(mSize.y() - s_kBorderWidth));
		nvgFillColor(ctx, nvgRGBA(255, 0, 0, 255));
		nvgFill(ctx);
	}

	nvgRestore(ctx);

	Widget::draw(ctx);
}

bool Node::mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers)
{
	if ((button == GLFW_MOUSE_BUTTON_1 || button == GLFW_MOUSE_BUTTON_2) && down) {
		m_navPainter.paintEvent(button, this);

		return true;
	}

	return Widget::mouseButtonEvent(p, button, down, modifiers);
}

bool Node::mouseEnterEvent(const Vector2i& p, bool enter)
{
	if (enter) {
		int button1State = glfwGetMouseButton(this->screen()->glfwWindow(), GLFW_MOUSE_BUTTON_1);
		int button2State = glfwGetMouseButton(this->screen()->glfwWindow(), GLFW_MOUSE_BUTTON_2);

		if (button1State == GLFW_PRESS)
			return m_navPainter.paintEvent(GLFW_MOUSE_BUTTON_1, this);
		if (button2State == GLFW_PRESS)
			return m_navPainter.paintEvent(GLFW_MOUSE_BUTTON_2, this);
	}

	return Widget::mouseEnterEvent(p, enter);
}

size_t Node::getRow() const
{
	return m_row;
}

size_t Node::getCol() const
{
	return m_col;
}

nanogui::Vector2i Node::getPos() const
{
	return absolutePosition();
}

nanogui::Vector2i Node::getSize() const
{
	return mSize;
}

void Node::setObstructed(bool isObstructed)
{
	m_obstructed = isObstructed;
}

bool Node::isObstructed() const
{
	return m_obstructed;
}

bool Node::connect(Node* node1, Node* node2)
{
	if (!node1 || !node2)
		return false;

	return node1->connect(node2);
}

bool Node::connect(Node* node)
{
	if (!node)
		return false;

	// Add connection to node if one doesn't already exist
	auto it = std::find(m_connections.begin(), m_connections.end(), node);
	if (it == m_connections.end()) {
		node->m_connections.push_back(this); // From that to this
		m_connections.push_back(node); // From this to that
		return true;
	}

	return false;
}

bool Node::removeConnection(Node* node)
{
	if (!node)
		return false;

	// Find and remove connection
	auto it = m_connections.begin();
	while (it != m_connections.end()) {
		if (*it == node) {
			if (removeConnection(it) != m_connections.end())
				return true;
			else
				return false;
		} else {
			++it;
		}
	}

	return false;
}

bool Node::removeConnection(Node* node1, Node* node2)
{
	if (!node1 || !node2)
		return false;

	return node1->removeConnection(node2);
}

std::list<Node*>::iterator Node::removeConnection(std::list<Node*>::iterator nodeIt)
{
	// Remove connection from specified node to this node
	for (auto it = (*nodeIt)->m_connections.begin(); it != (*nodeIt)->m_connections.end(); ++it) {
		if (*it == this) {
			(*nodeIt)->m_connections.erase(it);
			break;
		}
	}

	// Remove connection from this node to specified node
	return m_connections.erase(nodeIt);
}

std::list<Node*>::iterator Node::getConnectionListBegin()
{
	return m_connections.begin();
}

std::list<Node*>::iterator Node::getConnectionListEnd()
{
	return m_connections.end();
}
