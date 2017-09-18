#include <vector>
#include <chrono>
#include <thread>
#include <math.h>
#include <functional>

#include "PathFinder.h"
#include "Node.h"


PathFinder::PathFinder(nanogui::Widget* parent, nanogui::Window* parentWindow)
	: Window(parent, "")
	, m_parentWindow{ parentWindow }
	, m_stopped{ true }
{
}


PathFinder::~PathFinder()
{
}

void PathFinder::setStartNode(Node* node)
{
	if (!node)
		return;

	stop();
	m_startNode = node;
}

void PathFinder::setEndNode(Node* node)
{
	if (!node)
		return;

	stop();
	m_endNode = node;
}

bool PathFinder::isStart(const Node * node) const
{
	if (!node)
		return false;

	return node == m_startNode;
}

bool PathFinder::isEnd(const Node * node) const
{
	if (!node)
		return false;

	return node == m_endNode;
}

void PathFinder::calculatePath()
{
	if (!m_startNode || !m_endNode)
		return;

	if (!m_stopped)
		stop();

	m_stopped = false;

	// Add starting values to search graph
	m_frontier.emplace(m_startNode, 0);
	m_cameFrom[m_startNode] = nullptr;
	m_costSoFar[m_startNode] = 0;

	m_curNode = nullptr;
	while (!m_frontier.empty() && !m_stopped) {
		m_curNode = getNode(m_frontier.top());
		m_frontier.pop();
		
		if (m_curNode == m_endNode)
			break;

		// Explore neighbor nodes
		auto nextIt = m_curNode->getConnectionListBegin();
		auto end = m_curNode->getConnectionListEnd();
		for ( ; nextIt != end && !m_stopped; ++nextIt) {
			m_nextNode = *nextIt;
			float pathCost = getLinkCost(m_curNode, m_nextNode) + m_costSoFar.at(m_curNode);

			// Check if we should add / update node in search graph
			auto existingCostIt = m_costSoFar.find(m_nextNode);
			bool notExplored = existingCostIt == m_costSoFar.end();
			if (notExplored || pathCost < getCost(*existingCostIt)) {
				// Update path graph
				m_cameFrom[m_nextNode] = m_curNode;
				m_costSoFar[m_nextNode] = pathCost;

				// Calculate priority and add to frontier
				float priority = pathCost + heuristic(*m_nextNode, *m_endNode);
				m_frontier.emplace(m_nextNode, priority);

				std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}
		}
	}

	m_stopped = true;
}

void PathFinder::calculatePathAsync()
{
	stop();

	m_future = std::async(std::bind(&PathFinder::calculatePath, this));
}

void PathFinder::stop()
{
	if (!m_stopped) {
		m_stopped = true;
		m_future.wait();
	}
	clear();
}

void PathFinder::clear()
{
	m_curNode = nullptr;
	m_nextNode = nullptr;
	m_frontier = {};
	m_cameFrom.clear();
	m_costSoFar.clear();
}

void PathFinder::fillNode(NVGcontext* ctx, const Node& node, const NVGcolor& color)
{
	nvgBeginPath(ctx);
	nvgRect(ctx,
	        node.getPos().x() + Node::s_kBorderWidth / 2,
	        node.getPos().y() + Node::s_kBorderWidth / 2,
	        node.getSize().x() - Node::s_kBorderWidth,
	        node.getSize().y() - Node::s_kBorderWidth);
	nvgFillColor(ctx, color);
	nvgFill(ctx);
}

void PathFinder::strokeNode(NVGcontext* ctx, const Node& node, const NVGcolor& color)
{
	nvgBeginPath(ctx);
	nvgRect(ctx,
	        static_cast<float>(node.getPos().x()),
	        static_cast<float>(node.getPos().y()),
	        static_cast<float>(node.getSize().x()),
	        static_cast<float>(node.getSize().y()));
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
	float startX = nodeFrom.getPos().x() + nodeFrom.getSize().x() / 2.0f;
	float startY = nodeFrom.getPos().y() + nodeFrom.getSize().y() / 2.0f;
	nvgMoveTo(ctx, startX, startY);
	// Draw to center of connected node
	float endX = nodeTo.getPos().x() + nodeTo.getSize().x() / 2.0f;
	float endY = nodeTo.getPos().y() + nodeTo.getSize().y() / 2.0f;
	nvgLineTo(ctx, endX, endY);
	nvgStroke(ctx);
}

void PathFinder::draw(NVGcontext* ctx)
{
	refreshRelativePlacement();

	// Outline current node
	if (m_curNode)
		strokeNode(ctx, *m_curNode, nvgRGBA(255, 255, 0, 255));

	// Highlight nodes
	if (m_startNode)
		fillNode(ctx, *m_startNode, nvgRGBA(0, 100, 200, 180));
	if (m_endNode)
		fillNode(ctx, *m_endNode, nvgRGBA(100, 0, 200, 180));
	if (m_nextNode)
		fillNode(ctx, *m_nextNode, nvgRGBA(0, 0, 100, 225));

	for (auto it = m_cameFrom.begin(); it != m_cameFrom.end(); ++it) {
		Node* nodeTo = it->first;

		if (nodeTo && nodeTo != m_startNode && nodeTo != m_endNode && nodeTo != m_nextNode)
			fillNode(ctx, *nodeTo, nvgRGBA(0, 0, 100, 100));
	}

	// Draw current search graph
	for (auto it = m_cameFrom.begin(); it != m_cameFrom.end(); ++it) {
		Node* nodeFrom = it->second;
		Node* nodeTo = it->first;

		// Draw search graph connections
		if (nodeFrom && nodeTo) {
			drawGraphSegment(ctx, *nodeFrom, *nodeTo, nvgRGBA(255, 255, 255, 200));
		}
	}

	// Draw final path
	if (m_curNode && m_curNode == m_endNode) {
		Node* toNode = m_curNode;
		Node* cameFrom = m_cameFrom.at(m_curNode);
		while (toNode != nullptr) {
			fillNode(ctx, *toNode, nvgRGBA(255, 255, 0, 150));
			if (cameFrom)
				drawGraphSegment(ctx, *cameFrom, *toNode, nvgRGBA(255, 255, 0, 255));
			toNode = cameFrom;
			if (cameFrom)
				cameFrom = m_cameFrom.at(cameFrom);
		}
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
	return std::abs(static_cast<float>(nodeDst.getRow()) - static_cast<float>(nodeSrc.getRow()))
	     + std::abs(static_cast<float>(nodeDst.getCol()) - static_cast<float>(nodeSrc.getCol()));
}

float PathFinder::euclideanDist(const Node& nodeSrc, const Node& nodeDst)
{
	return sqrt(std::pow(static_cast<float>(nodeDst.getRow()) - static_cast<float>(nodeSrc.getRow()), 2)
	          + std::pow(static_cast<float>(nodeDst.getCol()) - static_cast<float>(nodeSrc.getCol()), 2));
}

Node* PathFinder::getNode(const NodePriorityPair& nodeCostPair)
{
	return nodeCostPair.first;
}

float PathFinder::getCost(const NodePriorityPair& nodeCostPair)
{
	return nodeCostPair.second;
}

bool PathFinder::PriorityComparator::operator()(const NodePriorityPair& lhs, const NodePriorityPair& rhs) const
{
	return getCost(lhs) > getCost(rhs);
}
