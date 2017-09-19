#include <vector>
#include <chrono>
#include <math.h>
#include <functional>
#include <algorithm>

#include "PathFinder.h"
#include "Node.h"
#include "Utils.h"


PathFinder::PathFinder(nanogui::Widget* parent, nanogui::Window* parentWindow)
	: Window(parent, "")
	, m_parentWindow{ parentWindow }
	, m_stopped{ true }
	, m_traversalList{ std::make_unique<std::vector<Node*>>() }
{
}


PathFinder::~PathFinder()
{
	if (m_processingThread.joinable()) {
		m_stopped = true;
		m_processingThread.join();
	}
}

void PathFinder::addNode(Node * node)
{
	m_stopped = true;
	std::lock_guard<std::mutex> lock{ m_mutex };

	m_traversalList->push_back(node);
}

void PathFinder::removeNode(Node* deleteNode)
{
	m_stopped = true;
	std::lock_guard<std::mutex> lock{ m_mutex };

	m_traversalList->erase(
		std::remove_if(
			m_traversalList->begin(),
			m_traversalList->end(),
			[deleteNode](Node* node) {
				return node == deleteNode;
	}));
}

void PathFinder::calculatePath()
{
	// Stop any existing processing
	m_stopped = true;
	std::lock_guard<std::mutex> lock{ m_mutex };

	m_stopped = false;
	m_newOrder = std::make_unique<std::vector<Node*>>(*m_traversalList);

	auto it1 = select_randomly(m_newOrder->begin(), m_newOrder->end());
	auto it2 = select_randomly(m_newOrder->begin(), m_newOrder->end());
	std::swap(*it1, *it2);

	std::lock_guard<std::mutex> listLock(m_listMutex);
	std::swap(m_traversalList, m_newOrder);

	m_stopped = true;
}

void PathFinder::calculatePathAsync()
{
	if (m_processingThread.joinable()) {
		m_stopped = true;
		m_processingThread.join();
	}

	m_processingThread = std::thread(std::bind(&PathFinder::calculatePath, this));
}

void PathFinder::fillNode(NVGcontext* ctx, const Node& node, const NVGcolor& color)
{
	nvgBeginPath(ctx);
	nvgRect(ctx,
	        node.absolutePosition().x() + Node::s_kBorderWidth / 2,
	        node.absolutePosition().y() + Node::s_kBorderWidth / 2,
	        node.size().x() - Node::s_kBorderWidth,
	        node.size().y() - Node::s_kBorderWidth);
	nvgFillColor(ctx, color);
	nvgFill(ctx);
}

void PathFinder::strokeNode(NVGcontext* ctx, const Node& node, const NVGcolor& color)
{
	nvgBeginPath(ctx);
	nvgRect(ctx,
	        static_cast<float>(node.absolutePosition().x()),
	        static_cast<float>(node.absolutePosition().y()),
	        static_cast<float>(node.size().x()),
	        static_cast<float>(node.size().y()));
	nvgStrokeWidth(ctx, 4.0f);
	nvgStrokeColor(ctx, color);
	nvgStroke(ctx);
}

void PathFinder::drawGraphSegment(NVGcontext* ctx, const Node& nodeFrom, const Node& nodeTo, const NVGcolor& color)
{
	nvgStrokeWidth(ctx, 3.0f);
	nvgStrokeColor(ctx, color);
	nvgBeginPath(ctx);
	// Move to center of node
	float startX = nodeFrom.absolutePosition().x() + nodeFrom.size().x() / 2.0f;
	float startY = nodeFrom.absolutePosition().y() + nodeFrom.size().y() / 2.0f;
	nvgMoveTo(ctx, startX, startY);
	// Draw to center of connected node
	float endX = nodeTo.absolutePosition().x() + nodeTo.size().x() / 2.0f;
	float endY = nodeTo.absolutePosition().y() + nodeTo.size().y() / 2.0f;
	nvgLineTo(ctx, endX, endY);
	nvgStroke(ctx);
}

void PathFinder::draw(NVGcontext* ctx)
{
	refreshRelativePlacement();

	std::lock_guard<std::mutex> lock{ m_listMutex };
	for (size_t i = 0; m_traversalList && i < m_traversalList->size(); ++i) {
		Node& nodeFrom = *m_traversalList->at(i);
		Node& nodeTo = *m_traversalList->at((i + 1) % m_traversalList->size());
		drawGraphSegment(ctx, nodeFrom, nodeTo, nvgRGBA(255, 255, 255, 255));
	}
}

void PathFinder::refreshRelativePlacement()
{
	if (m_parentWindow) {
		mPos = m_parentWindow->position();
		mSize = m_parentWindow->size();
	}
}

bool PathFinder::mouseButtonEvent(const nanogui::Vector2i & p, int button, bool down, int modifiers)
{
	return false;
}

bool PathFinder::mouseEnterEvent(const nanogui::Vector2i & p, bool enter)
{
	return false;
}

float PathFinder::getLinkCost(const Node* nodeSrc, const Node* nodeDst)
{
	if (!nodeSrc || !nodeDst)
		return 0;

	return euclideanDist(*nodeSrc, *nodeDst);
}

float PathFinder::heuristic(const Node& nodeSrc, const Node& nodeDst)
{
	return euclideanDist(nodeSrc, nodeDst);
}

float PathFinder::manhattanDist(const Node& nodeSrc, const Node& nodeDst)
{
	return std::abs(static_cast<float>(nodeDst.absolutePosition().x()) - static_cast<float>(nodeSrc.size().x()))
	     + std::abs(static_cast<float>(nodeDst.absolutePosition().y()) - static_cast<float>(nodeSrc.size().y()));
}

float PathFinder::euclideanDist(const Node& nodeSrc, const Node& nodeDst)
{
	return sqrt(std::pow(static_cast<float>(nodeDst.absolutePosition().x()) - static_cast<float>(nodeSrc.size().x()), 2)
	          + std::pow(static_cast<float>(nodeDst.absolutePosition().y()) - static_cast<float>(nodeSrc.size().y()), 2));
}