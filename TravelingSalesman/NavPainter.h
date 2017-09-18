#pragma once

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

	NavPainter(Grid& grid, PathFinder& pathFinder);
	~NavPainter();

	bool paintEvent(int button, Node* node);
	void setCurrentBrush(BrushType brush);

private:
	void paintObstacle(Node& node);
	void clearObstacle(Node& node);

	BrushType m_currentBrush;
	Grid& m_grid;
	PathFinder& m_pathFinder;
};

