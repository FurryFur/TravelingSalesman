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
	enum Mode {
		Anealing,
		HillClimbing
	};


	PathFinder();
	~PathFinder();

	void addNode(Node* node);
	void removeNode(Node* node);
	void calculatePath();
	void calculatePathAsync();
	void setMode(Mode mode);

	// Sets the temperature decay per second, expressed as a percentage of the 
	// current temperature.
	// E.g. 0.5 would be a 50% decrease in temperature per second.
	void setTemperatureDecay(double tempDecay);

	// Gets the temperature decay per second, expressed as a percentage of the 
	// current temperature.
	// E.g. 0.5 would be a 50% decrease in temperature per second.
	double getTemperatureDecay();

	// Returns true if pathing had to be halted.
	// Returns false if pathing was not calculating.
	bool stop();

	bool isStopped();

	void draw(NVGcontext* ctx);
	
private:
	static double euclideanDistSquared(const Node&, const Node&);
	static double euclideanDist(const Node&, const Node&);
	static double calculatePathLength(const std::vector<Node*>&);

	double calculateAcceptanceProbability(double candidatePathLength);
	void drawGraphSegment(NVGcontext* ctx, const Node& nodeFrom, const Node& nodeTo, const NVGcolor& color);

	static const double s_kStartingTemperature;

	std::vector<Node*> m_path;
	double m_pathLength;
	double m_temperature;
	double m_tempDecay;
	bool m_stopped;
	double m_pathsPerSecond;
	double m_avgAcceptanceProb;
	std::mutex m_mutex;
	std::thread m_processingThread;
	Mode m_mode;
};