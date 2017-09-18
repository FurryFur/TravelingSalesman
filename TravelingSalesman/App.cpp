#include <future>

#include "App.h"
#include "Node.h"
#include "CustomButton.h"

using namespace nanogui;

AStarApp::AStarApp()
	: Screen(Vector2i(1500, 850), "AStar")
	, m_modulation{ 5 }
	, m_grid{}
{
	/**
	* Add a window.
	* To the window add a label and a slider widget.
	*/

	Window* window = new Window(this, "");
	window->setPosition({ 15, 15 });
	window->setLayout(new GroupLayout(5, 5, 0, 0));

	Label* l = new Label(window, "MODULATION", "sans-bold");
	l->setFontSize(10);
	auto slider = new Slider(window);
	slider->setValue(0.5f);
	slider->setCallback([this](float value) { m_modulation = value * 10.0f; });

	Window* window2 = new Window(this, "");
	window2->setPosition({ 100, 15 });
	window2->setLayout(new GridLayout(Orientation::Horizontal, Node::s_kGridSize));

	// Instantiate path finder and its UI components
	auto pathFinder = new PathFinder(this, window2);
	m_navPainter = std::make_unique<NavPainter>(m_grid, *pathFinder);

	// Create pathing nodes
	for (size_t i = 0; i < Node::s_kGridSize; ++i) {
		for (size_t j = 0; j < Node::s_kGridSize; ++j) {
			auto node = new Node(window2, *m_navPainter, i, j);
			node->setFixedSize({ 50, 50 });
			m_grid.setGridNode(i, j, node);
		}
	}
	
	// Add connections to nodes
	for (size_t r = 0; r < Node::s_kGridSize; ++r) {
		for (size_t c = 0; c < Node::s_kGridSize; ++c) {
			for (int relR = -1; relR <= 1; ++relR) {
				for (int relC = -1; relC <= 1; ++relC) {
					if (relR == 0 && relC == 0)
						continue;

					size_t connRow = r + relR;
					size_t connCol = c + relC;

					Node::connect(m_grid[r][c], m_grid[connRow][connCol]);
				}
			}
		}
	}

	// Setup the simulate button
	Window* window3 = new Window(this, "");
	window3->setPosition({ 920, 685 });
	window3->setLayout(new GroupLayout());
	auto button = new Button(window3, "SIMULATE");
	button->setBackgroundColor(Color(255, 0, 0, 1));
	button->setFixedSize({ 500, 100 });
	button->setCallback([pathFinder]() {
		pathFinder->calculatePathAsync();
	});

	// Setup brush pallet
	Window* toolsWindow = new Window(this, "Brush");
	toolsWindow->setPosition({ 907, 15 });
	toolsWindow->setLayout(new GroupLayout());
	auto placeStartTool = new CustomButton(toolsWindow, "Start");
	placeStartTool->setPushed(true);
	placeStartTool->setFlags(Button::RadioButton);
	placeStartTool->setCallback([this]() {
		m_navPainter->setCurrentBrush(NavPainter::BrushType::Start);
	});
	auto placeEndTool = new CustomButton(toolsWindow, "End");
	placeEndTool->setFlags(Button::RadioButton);
	placeEndTool->setCallback([this]() {
		m_navPainter->setCurrentBrush(NavPainter::BrushType::End);
	});
	auto placeObstruction = new CustomButton(toolsWindow, "Obstacle");
	placeObstruction->setFlags(Button::RadioButton);
	placeObstruction->setCallback([this]() {
		m_navPainter->setCurrentBrush(NavPainter::BrushType::Obstacle);
	});

	// Do the layout calculations based on what was added to the GUI
	performLayout();

	// Load GLSL shader code
	m_shader.initFromFiles("raymarching_shader", "Shaders/vert.glsl", "Shaders/frag.glsl");

	/**
	* Fill the screen with a rectangle (2 triangles)
	*/
	MatrixXu indices(3, 2);
	indices.col(0) << 0, 1, 2;
	indices.col(1) << 2, 1, 3;

	MatrixXf positions(3, 4);
	positions.col(0) << -1, -1, 0;
	positions.col(1) << 1, -1, 0;
	positions.col(2) << -1, 1, 0;
	positions.col(3) << 1, 1, 0;

	// bind the shader and upload vertex positions and indices
	m_shader.bind();
	m_shader.uploadIndices(indices);
	m_shader.uploadAttrib("a_position", positions);

	// Set initial value for modulation uniform
	m_shader.setUniform("modulation", m_modulation);

	// Set resolution and screenRatio uniforms
	int fboWidth, fboHeight;
	glfwGetFramebufferSize(mGLFWWindow, &fboWidth, &fboHeight);
	m_shader.setUniform("resolution", Vector2f{ fboWidth, fboHeight });
	float mx = std::max<float>(static_cast<float>(fboWidth), static_cast<float>(fboHeight));
	auto xDim = fboWidth / mx;
	auto yDim = fboHeight / mx;
	m_shader.setUniform("screenRatio", Vector2f{ xDim, yDim });
}

void AStarApp::drawContents()
{
	m_shader.bind();
	m_shader.setUniform("modulation", m_modulation);
	m_shader.drawIndexed(GL_TRIANGLES, 0, 2);
}

void AStarApp::draw(NVGcontext * ctx)
{
	//nvgBeginPath(ctx);
	//nvgRect(ctx, 100, 100, 120, 30);
	//nvgCircle(ctx, 120, 120, 5);
	//nvgPathWinding(ctx, NVG_HOLE);	// Mark circle as a hole.
	//nvgFillColor(ctx, nvgRGBA(255, 192, 0, 255));
	//nvgFill(ctx);

	Screen::draw(ctx);
}
