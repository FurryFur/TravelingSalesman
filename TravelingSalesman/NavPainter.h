#pragma once

#include <nanogui\object.h>

class Node;
class Grid;
class PathFinder;

class NavPainter {
public:
	enum BrushType {
		Start,
		End,
		Obstacle
	};

	NavPainter(Grid& grid, nanogui::ref<PathFinder> pathFinder);
	~NavPainter();

	bool paintEvent(int button, nanogui::ref<Node> node);
	void setCurrentBrush(BrushType brush);

private:
	void paintObstacle(Node& node);
	void clearObstacle(Node& node);

	BrushType m_currentBrush;
	Grid& m_grid;
	nanogui::ref<PathFinder> m_pathFinder;
};

