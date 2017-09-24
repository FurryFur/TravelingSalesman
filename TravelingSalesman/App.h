//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Top level application class. 
//                Lays out UI components and initializes system sub-components.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <nanogui\nanogui.h>

#include "PathFinder.h"

class TravelingSalesmanApp : public nanogui::Screen {
public:
	// Setup UI and Shader
	TravelingSalesmanApp();

	// Draw opengl stuff (screen will handle clear and buffer swap)
	void drawContents() override;

	// Draw nanonvg (vector graphics) stuff
	void draw(NVGcontext* ctx) override;

private:
	nanogui::GLShader m_shader;
	float m_modulation;
	PathFinder m_pathFinder;
};