#include <nanogui\object.h>

#include "NavPainter.h"
#include "Node.h"
#include "Grid.h"
#include "PathFinder.h"

NavPainter::NavPainter(Grid& grid, nanogui::ref<PathFinder> pathfinder)
	: m_grid{ grid }
	, m_pathFinder{ std::move(pathfinder) }
	, m_currentBrush{ BrushType::Start }
{
}


NavPainter::~NavPainter()
{
}

bool NavPainter::paintEvent(int button, nanogui::ref<Node> node)
{
	if (!node)
		return false;

	bool startOrEnd = m_pathFinder->isStart(node) || m_pathFinder->isEnd(node);

	switch (m_currentBrush) {
	case NavPainter::Start:
		if (!startOrEnd && !node->isObstructed()) {
			m_pathFinder->setStartNode(std::move(node));
			return true; // Handled / state change occured
		}
		break;
	case NavPainter::End:
		if (!startOrEnd && !node->isObstructed()) {
			m_pathFinder->setEndNode(std::move(node));
			return true; // Handled / state change occured
		}
		break;
	case NavPainter::Obstacle:
		if (!node->isObstructed() && !startOrEnd && button == GLFW_MOUSE_BUTTON_1) {
			paintObstacle(*node);
			return true; // Handled / state change occured
		}

		if (node->isObstructed() && !startOrEnd && button == GLFW_MOUSE_BUTTON_2) {
			clearObstacle(*node);
			return true; // Handled / state change occured
		}
		break;
	}

	return false;
}

void NavPainter::setCurrentBrush(BrushType brush)
{
	m_currentBrush = brush;
}

void NavPainter::paintObstacle(Node& node)
{
	m_pathFinder->stop();
	node.setObstructed(true);

	// Remove cardinal nodes' diagonal connections around this node
	size_t row = node.getRow();
	size_t col = node.getCol();
	Node::removeConnection(m_grid[row - 1][col], m_grid[row][col - 1]);
	Node::removeConnection(m_grid[row][col - 1], m_grid[row + 1][col]);
	Node::removeConnection(m_grid[row + 1][col], m_grid[row][col + 1]);
	Node::removeConnection(m_grid[row][col + 1], m_grid[row - 1][col]);

	// Remove connections with this node
	auto it = node.getConnectionListBegin();
	while (it != node.getConnectionListEnd()) {
		it = node.removeConnection(it);
	}
}

void NavPainter::clearObstacle(Node& node)
{
	m_pathFinder->stop();
	node.setObstructed(false);

	// Reconnect the now unobstructed node
	for (int relR = -1; relR <= 1; ++relR) {
		for (int relC = -1; relC <= 1; ++relC) {
			if (relR == 0 && relC == 0)
				continue;

			size_t row = node.getRow() + relR;
			size_t col = node.getCol() + relC;
			nanogui::ref<Node> nodeTo = m_grid[row][col];

			if (m_grid.areConnectable(&node, nodeTo))
				node.connect(nodeTo);
		}
	}

	// Recconnect cardinal nodes' diagonal connections around this node
	size_t row = node.getRow();
	size_t col = node.getCol();
	nanogui::ref<Node> node1 = m_grid[row - 1][col];
	nanogui::ref<Node> node2 = m_grid[row][col - 1];
	nanogui::ref<Node> node3 = m_grid[row + 1][col];
	nanogui::ref<Node> node4 = m_grid[row][col + 1];
	if (m_grid.areConnectable(node1, node2))
		Node::connect(node1, node2);
	if (m_grid.areConnectable(node2, node3))
		Node::connect(node2, node3);
	if (m_grid.areConnectable(node3, node4))
		Node::connect(node3, node4);
	if (m_grid.areConnectable(node4, node1))
		Node::connect(node4, node1);
}
