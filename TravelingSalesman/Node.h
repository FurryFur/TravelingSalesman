#pragma once

#include <list>
#include <memory>

#include <nanogui/nanogui.h>

class Grid;
class NavPainter;

class Node : public nanogui::Widget {
public:
	Node(nanogui::Window* window, std::shared_ptr<NavPainter> navPainter, size_t row, size_t col);

	// Draws the cell.
	virtual void draw(NVGcontext* ctx) override;

	// Handles clicking on the cell.
	virtual bool mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers) override;

	// Handles entering the cell with a mouse button down.
	virtual bool mouseEnterEvent(const nanogui::Vector2i& p, bool enter) override;

	// Adds a pathing connection to the specified node.
	// Returns true if a new connection was made.
	// Returns false if the connection already exists or the specified node is null.
	bool connect(nanogui::ref<Node> node);

	size_t getRow() const;
	size_t getCol() const;
	nanogui::Vector2i getPos() const;
	nanogui::Vector2i getSize() const;
	void setObstructed(bool isObstructed);
	bool isObstructed() const;

	// Adds a pathing connection between the specified nodes.
	// Returns true if a new connection was made.
	// Returns false if the connection already exists or either of the specified nodes are null.
	static bool connect(nanogui::ref<Node> node1, nanogui::ref<Node> node2);

	// Removes the pathing connection to the specified node.
	// Returns false if no connection was found or specified node was null.
	bool removeConnection(Node* node);

	// Removes the pathing connection between specified nodes.
	// Returns false if no connection was found or either of the specified nodes are null.
	static bool removeConnection(Node* node1, Node* node2);

	// Removes the pathing connection to specified node via an iterator.
	// Returns an iterator pointing to the connection following the one that was removed.
	std::list<nanogui::ref<Node>>::iterator removeConnection(std::list<nanogui::ref<Node>>::iterator nodeIt);

	// Returns an iterator pointing to the beginning of the connection list
	std::list<nanogui::ref<Node>>::iterator getConnectionListBegin();

	// Returns an iterator point to the end of the connection list
	std::list<nanogui::ref<Node>>::iterator getConnectionListEnd();

	static const size_t s_kGridSize = 16;
	static const float s_kBorderWidth;
private:
	bool m_obstructed;
	std::shared_ptr<NavPainter> m_navPainter;
	std::list<nanogui::ref<Node>> m_connections;
	size_t m_row;
	size_t m_col;
};