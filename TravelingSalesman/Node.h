#pragma once

#include <list>
#include <memory>
#include <functional>

#include <nanogui\nanogui.h>

class Grid;
class NavPainter;

class Node : public nanogui::Widget {
public:
	Node(nanogui::Widget* parent);
	virtual ~Node() override;

	// Draws the cell.
	virtual void draw(NVGcontext* ctx) override;

	using CallbackT = std::function<void(const nanogui::Vector2i& p, int button, bool down, int modifiers)>;

	void setCallback(CallbackT callback);
	virtual bool mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers) override;
	virtual bool mouseDragEvent(const nanogui::Vector2i& p, const nanogui::Vector2i& rel, int button, int modifiers) override;

	// Adds a pathing connection to the specified node.
	// Returns true if a new connection was made.
	// Returns false if the connection already exists or the specified node is null.
	bool connect(Node* node);

	// Adds a pathing connection between the specified nodes.
	// Returns true if a new connection was made.
	// Returns false if the connection already exists or either of the specified nodes are null.
	static bool connect(Node* node1, Node* node2);

	// Removes the pathing connection to the specified node.
	// Returns false if no connection was found or specified node was null.
	bool removeConnection(Node* node);

	// Removes the pathing connection between specified nodes.
	// Returns false if no connection was found or either of the specified nodes are null.
	static bool removeConnection(Node* node1, Node* node2);

	// Removes the pathing connection to specified node via an iterator.
	// Returns an iterator pointing to the connection following the one that was removed.
	std::list<Node*>::iterator removeConnection(std::list<Node*>::iterator nodeIt);

	// Returns an iterator pointing to the beginning of the connection list
	std::list<Node*>::iterator getConnectionListBegin();

	// Returns an iterator point to the end of the connection list
	std::list<Node*>::iterator getConnectionListEnd();

	static const float s_kBorderWidth;
private:
	std::list<Node*> m_connections;
	CallbackT m_callback;
};