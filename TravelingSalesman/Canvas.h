//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : The main canvas where nodes in the traveling salesman 
//                problem can be placed, moved around, and deleted.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <nanogui\widget.h>

class Node;
class PathFinder;

class Canvas : public nanogui::Widget {
public:
	Canvas(nanogui::Widget* parent, PathFinder& pathFinder);

	// Handles various mouse click events on the canvas
	bool mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers) override;

	// Draws the canvas, nodes, and current solver (PathFinder) state.
	void draw(NVGcontext* ctx) override;

	std::vector<Node*> nodes;
	PathFinder& m_pathFinder;
};

