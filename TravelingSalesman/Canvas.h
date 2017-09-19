#pragma once

#include <string>

#include <nanogui\widget.h>

class Node;
class PathFinder;

class Canvas : public nanogui::Widget {
public:
	Canvas(nanogui::Widget* parent, PathFinder& pathFinder);
	virtual ~Canvas();

	virtual bool mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers) override;
	std::vector<Node*> nodes;
	PathFinder& m_pathFinder;
};

