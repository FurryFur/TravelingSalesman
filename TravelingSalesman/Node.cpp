#include <algorithm>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include "Node.h"

using namespace nanogui;

const float Node::s_kBorderWidth = 4;

Node::Node(Widget* parent)
	: Widget(parent)
{
	mSize = { 10, 10 };
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

	nvgRestore(ctx);

	Widget::draw(ctx);
}

void Node::setCallback(CallbackT callback)
{
	m_callback = callback;
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
	if (buttonState & 0x1) { // Not GLFW mouse button apparently (seems to have 1 bit set, going from right to left, for each mouse button pressed)
		mPos += rel;
		return true;
	}

	return false;
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
