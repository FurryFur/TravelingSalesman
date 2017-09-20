#include <vector>
#include <chrono>
#include <math.h>
#include <functional>
#include <algorithm>
#include <string>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include <nanovg.h>

#include "PathFinder.h"
#include "Node.h"
#include "Utils.h"


PathFinder::PathFinder()
	: m_stopped{ true }
	, m_path{}
	, m_pathLength{ 0 }
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
	m_pathLength = calculatePathLength(m_path);
}

void PathFinder::removeNode(Node* node)
{
	stop();

	std::lock_guard<std::mutex> lock{ m_mutex };
	unorderedErase(
		m_path,
		std::find(
			m_path.begin(),
			m_path.end(),
			node));
	m_pathLength = calculatePathLength(m_path);
}

void PathFinder::calculatePath()
{
	if (m_path.size() <= 0)
		return;

	std::vector<Node*> newPath = m_path;

#ifdef _DEBUG
	std::cout << "Pathing started" << std::endl;
#endif // _DEBUG

	m_pathLength = calculatePathLength(m_path);

	// Start timing
	unsigned long long pathsProcessed = 0;
	auto begin = std::chrono::high_resolution_clock::now();

	// Loop until stopped
	while (!m_stopped) {
		// Try swapping two nodes on the path
		auto it1 = selectRandomly(newPath.begin(), newPath.end());
		auto it2 = selectRandomly(newPath.begin(), newPath.end());
		std::swap(*it1, *it2);

		// Test if the new path length is better
		float newPathLength = calculatePathLength(newPath);
		if (newPathLength < m_pathLength) {
			// Update main path if better
			size_t i = it1 - newPath.begin();
			size_t j = it2 - newPath.begin();

			std::unique_lock<std::mutex> lock(m_mutex);
			std::swap(m_path.at(i), m_path.at(j));
			m_pathLength = newPathLength;
			lock.unlock();
		} else {
			// Undo changes if not
			std::swap(*it1, *it2);
		}

		// Get timing results
		++pathsProcessed;
		auto now = std::chrono::high_resolution_clock::now();
		auto timeSinceLastReport = std::chrono::duration_cast<std::chrono::microseconds>(now - begin);
		using namespace std::chrono_literals;
		if (timeSinceLastReport > 100ms) {
			m_pathsPerSecond = pathsProcessed / (timeSinceLastReport.count() / 1000000.0);
			begin = now;
			pathsProcessed = 0;
		}
	}

	m_stopped = true;
}

void PathFinder::calculatePathAsync()
{
	stop();

	m_stopped = false;
	m_processingThread = std::thread(std::bind(&PathFinder::calculatePath, this));
}

bool PathFinder::stop()
{
	bool result = false;

	m_stopped = true;
	if (m_processingThread.joinable()) {
		m_processingThread.join();

#ifdef _DEBUG
		std::cout << "Pathing stopped" << std::endl;
#endif // _DEBUG

		result = true;
	}

	// Always update path length on attempted stop
	m_pathLength = calculatePathLength(m_path);

	return result;
}

bool PathFinder::isStopped()
{
	return m_stopped;
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
	std::lock_guard<std::mutex> lock{ m_mutex };
	for (size_t i = 0; i < m_path.size(); ++i) {
		Node& nodeFrom = *m_path.at(i);
		Node& nodeTo = *m_path.at((i + 1) % m_path.size());
		drawGraphSegment(ctx, nodeFrom, nodeTo, nvgRGBA(255, 255, 255, 255));
	}

	// Draw current best path distance
	nvgFontFace(ctx, "sans");
	nvgFontSize(ctx, 24);
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	std::string distText = "Distance: " + toString(m_pathLength);
	nvgText(ctx, 10, 10, distText.c_str(), nullptr);

	// Draw timing stats
	nvgText(ctx, 10, 40, ("Paths per second: " + toString(m_pathsPerSecond)).c_str(), nullptr);
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
