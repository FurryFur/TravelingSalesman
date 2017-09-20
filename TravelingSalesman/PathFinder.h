#pragma once

#include <condition_variable>
#include <mutex>
#include <utility>
#include <queue>
#include <unordered_map>
#include <thread>
#include <memory>

#include <nanogui\nanogui.h>

class Node;

class PathFinder : public nanogui::Window {
public:
	PathFinder(nanogui::Widget* parent, nanogui::Window* parentWindow);
	~PathFinder();

	void addNode(Node* node);
	void removeNode(Node* node);
	void calculatePath();
	void calculatePathAsync();

	// Returns true if pathing had to be halted.
	// Returns false if pathing was not calculating.
	bool stop();

	bool isStopped();

	virtual void draw(NVGcontext* ctx) override;
	virtual void refreshRelativePlacement() override;

	// Don't handle mouse events / propogate to other widgets
	virtual bool mouseButtonEvent(const nanogui::Vector2i& p, int button, bool down, int modifiers) override;
	virtual bool mouseEnterEvent(const nanogui::Vector2i& p, bool enter) override;
	
private:
	static float euclideanDistSquared(const Node&, const Node&);
	static float calculatePathLength(const std::vector<Node*>&);

	void fillNode(NVGcontext* ctx, const Node& node, const NVGcolor& color);
	void strokeNode(NVGcontext* ctx, const Node& node, const NVGcolor& color);
	void drawGraphSegment(NVGcontext* ctx, const Node& nodeFrom, const Node& nodeTo, const NVGcolor& color);

	nanogui::Window* m_parentWindow;
	std::vector<Node*> m_path;
	//std::future<void> m_future;
	bool m_stopped;
	std::mutex m_mutex;
	std::thread m_processingThread;
};