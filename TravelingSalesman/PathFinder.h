#pragma once

#include <condition_variable>
#include <mutex>
#include <utility>
#include <queue>
#include <unordered_map>
#include <thread>
#include <memory>

namespace nanogui {
	class Widget;
}

class NVGContext;
class Node;

class PathFinder {
public:
	PathFinder();
	~PathFinder();

	void addNode(Node* node);
	void removeNode(Node* node);
	void calculatePath();
	void calculatePathAsync();

	// Returns true if pathing had to be halted.
	// Returns false if pathing was not calculating.
	bool stop();

	bool isStopped();

	void draw(NVGcontext* ctx);
	
private:
	static float euclideanDistSquared(const Node&, const Node&);
	static float calculatePathLength(const std::vector<Node*>&);

	void drawGraphSegment(NVGcontext* ctx, const Node& nodeFrom, const Node& nodeTo, const NVGcolor& color);

	std::vector<Node*> m_path;
	float m_pathLength;
	bool m_stopped;
	double m_pathsPerSecond;
	std::mutex m_mutex;
	std::thread m_processingThread;
};