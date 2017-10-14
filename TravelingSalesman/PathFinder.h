//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Solver for the traveling salesman problem.
//                Is able to draw current solution and statistics.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <vector>
#include <mutex>
#include <thread>

class NVGContext;
class Node;

class PathFinder {
public:
	enum Mode {
		Anealing,
		HillClimbing,
		Genetic
	};


	PathFinder();
	~PathFinder();

	// Adds a node to the solver.
	void addNode(Node* node);

	// Removes a node from the solver.
	void removeNode(Node* node);

	// Tries to solve the traveling salesman problem using the current mode 
	// (Hill Climbing or Simulated Annealing).
	// Note: This function runs an infinite loop and will never terminate 
	// unless it is run in a separate thread.
	void calculatePath();

	// Tries to solve the traveling salesman problem using the current mode 
	// (Hill Climbing or Simulated Annealing).
	// Note: This function creates a separate thread to run the solver function 
	// which runs in an infinite loop. The solver can be terminated by calling 
	// the stop member function.
	void calculatePathAsync();

	// Sets the mode (Hill Climbing or Simulated Anneadling) to run the solver 
	// functions (calculatePath or calculatePathAsync) in.
	void setMode(Mode mode);

	// Sets the temperature decay per second, expressed as a percentage of the 
	// current temperature.
	// E.g. 0.5 would be a 50% decrease in temperature per second.
	void setTemperatureDecay(double tempDecay);

	// Gets the temperature decay per second, expressed as a percentage of the 
	// current temperature.
	// E.g. 0.5 would be a 50% decrease in temperature per second.
	double getTemperatureDecay();

	// Halts any currently running solver.
	// Returns true if solver had to be halted.
	// Returns false if solver was not calculating.
	bool stop();

	// Returns true if no solver thread is running.
	bool isStopped();

	// Draws the current path configuration can statistics.
	void draw(NVGcontext* ctx);
	
private:
	// Returns the squared distance between two nodes.
	static double euclideanDistSquared(const Node&, const Node&);

	// Returns the distance between two nodes
	static double euclideanDist(const Node&, const Node&);

	// Calculates the total path length of a given configuration.
	static double calculatePathLength(const std::vector<Node*>&);

	// Returns the probability that a candidate configuration will 
	// be accepted as the new configuration.
	double calculateAcceptanceProbability(double candidatePathLength);

	// Draws a connecting line between two nodes
	void drawGraphSegment(NVGcontext* ctx, const Node& nodeFrom, const Node& nodeTo, const NVGcolor& color);

	// Helper function to perform anealing / hillclimbing.
	void doAnnealingHillclimbing();

	// Helper function to calculate path based on a genetic algorithm.
	void doGenetic();
	
	// Selects the best candidate path from a list
	const std::vector<Node*>& selectBest(const std::vector<std::vector<Node*>>& selectionPool);

	// Constructs a new path from two other parent paths
	std::vector<Node*> crossover(const std::vector<Node*>& parent1, const std::vector<Node*>& parent2);

	// Mutates the path in place and returns the result
	std::vector<Node*>& mutate(std::vector<Node*>& path);

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