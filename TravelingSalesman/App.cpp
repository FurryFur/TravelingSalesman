#include <future>

#include "App.h"
#include "Node.h"
#include "CustomButton.h"
#include "Canvas.h"
#include "PathFinder.h"

using namespace nanogui;

AStarApp::AStarApp()
	: Screen(Vector2i(1500, 850), "AStar")
	, m_modulation{ 5 }
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
	window2->setSize({ 800, 800 });
	window2->setId("Main Window");

	// Init main display
	auto canvas = new Canvas(window2, m_pathFinder);
	canvas->setSize(window2->size());
	canvas->setId("Canvas");

	// Setup the simulate button
	Window* window3 = new Window(this, "");
	window3->setPosition({ 920, 500 });
	window3->setLayout(new GroupLayout());
	auto button = new Button(window3, "SIMULATE");
	button->setBackgroundColor(Color(255, 0, 0, 1));
	button->setFixedSize({ 500, 100 });
	button->setCallback([this]() {
		m_pathFinder.calculatePathAsync();
	});

	// Setup stop
	Window* window4 = new Window(this, "");
	window4->setPosition({ 920, 685 });
	window4->setLayout(new GroupLayout());
	button = new Button(window4, "STOP");
	button->setBackgroundColor(Color(255, 0, 0, 1));
	button->setFixedSize({ 500, 100 });
	button->setCallback([this]() {
		m_pathFinder.stop();
	});

	// Setup mode select
	Window* modeWindow = new Window(this, "Mode");
	modeWindow->setPosition({ 907, 15 });
	modeWindow->setLayout(new GroupLayout());
	auto hillClimbingBtn = new CustomButton(modeWindow, "Hill Climbing");
	hillClimbingBtn->setPushed(true);
	hillClimbingBtn->setFlags(Button::RadioButton);
	hillClimbingBtn->setCallback([this]() {
		m_pathFinder.setMode(PathFinder::Mode::HillClimbing);
	});
	auto annealingBtn = new CustomButton(modeWindow, "Simulated Annealing");
	annealingBtn->setFlags(Button::RadioButton);
	annealingBtn->setCallback([this]() {
		m_pathFinder.setMode(PathFinder::Mode::Anealing);
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
