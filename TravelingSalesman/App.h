#pragma once

#include <future>
#include <memory>

#include <nanogui\nanogui.h>

#include "PathFinder.h"

class AStarApp : public nanogui::Screen {
public:
	// Setup UI and Shader
	AStarApp();

	// Draw opengl stuff (screen will handle clear and buffer swap)
	virtual void drawContents() override;

	// Draw nanonvg (vector graphics) stuff
	virtual void draw(NVGcontext* ctx) override;

private:
	nanogui::GLShader m_shader;
	float m_modulation;
	PathFinder m_pathFinder;
};