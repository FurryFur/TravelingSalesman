#include <vector>
#include <chrono>
#include <math.h>
#include <functional>
#include <algorithm>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG


#include "PathFinder.h"
#include "Node.h"
#include "Utils.h"


PathFinder::PathFinder(nanogui::Widget* parent, nanogui::Window* parentWindow)
	: Window(parent, "")
	, m_parentWindow{ parentWindow }
	, m_stopped{ true }
	, m_path{}
{
}


PathFinder::~PathFinder()
{
	stop();
}

void PathFinder::addNode(Node* node)
{
	stop();

	std::lock_guard<std::mutex> lock{ m_mutex };
	m_path.push_back(node);
}

void PathFinder::removeNode(Node* node)
{
	stop();

	std::lock_guard<std::mutex> lock{ m_mutex };
	unordered_erase(
		m_path,
		std::find(
			m_path.begin(),
			m_path.end(),
			node));
}

void PathFinder::calculatePath()
{
	m_stopped = false;
	std::vector<Node*> newPath = m_path;
	float bestPathLength = calculatePathLength(newPath);

#ifdef _DEBUG
	std::cout << "Pathing started" << std::endl;
#endif // _DEBUG


	// Loop until stopped
	while (!m_stopped) {
		// Try swapping two nodes on the path
		auto it1 = select_randomly(newPath.begin(), newPath.end());
		auto it2 = select_randomly(newPath.begin(), newPath.end());
		std::swap(*it1, *it2);

		// Test if the new path length is better
		float newPathLength = calculatePathLength(newPath);
		if (newPathLength < bestPathLength) {
			// Update main path if better
			bestPathLength = newPathLength;
			size_t i = it1 - newPath.begin();
			size_t j = it2 - newPath.begin();

			std::unique_lock<std::mutex> lock(m_mutex);
			std::swap(m_path.at(i), m_path.at(j));
			lock.unlock();
		} else {
			// Undo changes if not
			std::swap(*it1, *it2);
		}
	}

	m_stopped = true;
}

void PathFinder::calculatePathAsync()
{
	stop();

	m_processingThread = std::thread(std::bind(&PathFinder::calculatePath, this));
}

bool PathFinder::stop()
{
	if (!m_stopped) {
		m_stopped = true;
		if (m_processingThread.joinable()) {
			m_processingThread.join();
		}

#ifdef _DEBUG
		std::cout << "Pathing stopped" << std::endl;
#endif // _DEBUG

		return true;
	}

	return false;
}

bool PathFinder::isStopped()
{
	return m_stopped;
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
	nanogui::Vector2f start = nodeFrom.getFloatPos() + nodeFrom.getFloatSize() / 2;
	nvgMoveTo(ctx, start.x(), start.y());
	// Draw to center of connected node
	nanogui::Vector2f end = nodeTo.getFloatPos() + nodeTo.getFloatSize() / 2;
	nvgLineTo(ctx, end.x(), end.y());
	nvgStroke(ctx);
}

void PathFinder::draw(NVGcontext* ctx)
{
	refreshRelativePlacement();

	std::lock_guard<std::mutex> lock{ m_mutex };
	for (size_t i = 0; i < m_path.size(); ++i) {
		Node& nodeFrom = *m_path.at(i);
		Node& nodeTo = *m_path.at((i + 1) % m_path.size());
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

bool PathFinder::mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers)
{
	return false;
}

bool PathFinder::mouseEnterEvent(const nanogui::Vector2i& p, bool enter)
{
	return false;
}

float PathFinder::euclideanDistSquared(const Node& nodeSrc, const Node& nodeDst)
{
	nanogui::Vector2f displacement = nodeDst.getFloatPos() - nodeSrc.getFloatPos();
	return displacement.dot(displacement);
}

float PathFinder::calculatePathLength(const std::vector<Node*>& traversalList)
{
	float accumDist = 0;
	for (size_t i = 0; i < traversalList.size(); ++i) {
		Node& nodeFrom = *traversalList[i];
		Node& nodeto = *traversalList[(i + 1) % traversalList.size()];
		accumDist += euclideanDistSquared(nodeFrom, nodeto);
	}

	return accumDist;
}
