#pragma once

#include <future>
#include <utility>
#include <queue>
#include <unordered_map>

#include <nanogui\nanogui.h>

class Node;

class PathFinder : public nanogui::Window {
public:
	PathFinder(nanogui::Widget* parent, nanogui::ref<nanogui::Window> parentWindow);
	~PathFinder();

	void setStartNode(nanogui::ref<Node> node);
	void setEndNode(nanogui::ref<Node> node);
	bool isStart(const Node* node) const;
	bool isEnd(const Node* node) const;
	void calculatePath();
	void calculatePathAsync();
	void stop();

	virtual void draw(NVGcontext* ctx) override;
	virtual void refreshRelativePlacement() override;
	virtual bool mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers) override;
	virtual bool mouseEnterEvent(const nanogui::Vector2i& p, bool enter) override;
	
private:
	using NodePriorityPair = std::pair<Node*, float>;
	using PQContainerT = std::vector<NodePriorityPair>;

	class PriorityComparator {
	public:
		bool operator()(const NodePriorityPair& node1, const NodePriorityPair& node2) const;
	};

	static float getLinkCost(const Node*, const Node*);
	static float heuristic(const Node&, const Node&);
	static float manhattanDist(const Node&, const Node&);
	static float euclideanDist(const Node&, const Node&);

	static Node* getNode(const NodePriorityPair&);
	static float getCost(const NodePriorityPair&);

	void fillNode(NVGcontext* ctx, const Node& node, const NVGcolor& color);
	void strokeNode(NVGcontext* ctx, const Node& node, const NVGcolor& color);
	void drawGraphSegment(NVGcontext* ctx, const Node& nodeFrom, const Node& nodeTo, const NVGcolor& color);

	void clear();

	nanogui::ref<nanogui::Window> m_parentWindow;
	nanogui::ref<Node> m_startNode;
	nanogui::ref<Node> m_endNode;
	Node* m_curNode;
	Node* m_nextNode;
	std::priority_queue<NodePriorityPair, PQContainerT, PriorityComparator> m_frontier;
	std::unordered_map<Node*, Node*> m_cameFrom;
	std::unordered_map<Node*, float> m_costSoFar;
	std::future<void> m_future;
	bool m_stopped;
};