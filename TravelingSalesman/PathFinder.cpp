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

#include <chrono>
#include <math.h>
#include <algorithm>
#include <string>
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include <nanovg.h>
#include <Eigen\Dense>

#include "PathFinder.h"
#include "Node.h"
#include "Utils.h"

const double PathFinder::s_kStartingTemperature = 1000;

PathFinder::PathFinder()
	: m_stopped{ true }
	, m_path{}
	, m_pathLength{ 0 }
	, m_pathsPerSecond{ 0 }
	, m_generationsPerSecond{ 0 }
	, m_temperature{ s_kStartingTemperature }
	, m_mode{ HillClimbing }
	, m_tempDecay{ 1 }
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

#ifdef _DEBUG
	std::cout << "Pathing started" << std::endl;
#endif // _DEBUG

	if (m_mode == Mode::Genetic)
		doGenetic();
	else
		doAnnealingHillclimbing();


	m_stopped = true;
}

void PathFinder::doAnnealingHillclimbing()
{
	std::vector<Node*> newPath = m_path;
	m_pathLength = calculatePathLength(m_path);
	m_pathsPerSecond = 0;
	m_temperature = s_kStartingTemperature;

	// Start timing
	using namespace std::chrono;
	auto lastReportTime = high_resolution_clock::now();
	unsigned long long pathsProcessed = 0;
	unsigned long long acceptanceCalcCount = 0;
	double acceptanceProbSum = 0;

	// Loop until stopped
	while (!m_stopped) {
		auto begin = high_resolution_clock::now();

		// Try swapping two nodes on the path
		auto it1 = selectRandomly(newPath.begin(), newPath.end());
		auto it2 = selectRandomly(newPath.begin(), newPath.end());
		std::swap(*it1, *it2);

		// Decide whether to accept or discard the new candidate path
		double newPathLength = calculatePathLength(newPath);
		double acceptanceProb = calculateAcceptanceProbability(newPathLength);
		acceptanceProbSum += acceptanceProb;
		++acceptanceCalcCount;
		if (randomReal() < acceptanceProb) {
			// Update main path if the candidate path was accepted
			size_t i = it1 - newPath.begin();
			size_t j = it2 - newPath.begin();

			std::unique_lock<std::mutex> lock(m_mutex);
			std::swap(m_path.at(i), m_path.at(j));
			m_pathLength = newPathLength;
			lock.unlock();
		}
		else {
			// Revert path to previous if not accepted
			std::swap(*it1, *it2);
		}

		// Reduce temperature for simulated annealing
		if (m_mode == Anealing) {
			auto now = high_resolution_clock::now();
			double deltaT = duration_cast<nanoseconds>(now - begin).count() / 1000000000.0;
			m_temperature -= m_tempDecay * deltaT * m_temperature;
		}

		// Calculate current stats
		++pathsProcessed;
		auto now = high_resolution_clock::now();
		auto timeSinceLastReport = duration_cast<microseconds>(now - lastReportTime);
		using namespace std::chrono_literals;
		if (timeSinceLastReport > 100ms) {
			m_pathsPerSecond = pathsProcessed / (timeSinceLastReport.count() / 1000000.0);
			m_avgAcceptanceProb = acceptanceProbSum / acceptanceCalcCount;
			acceptanceCalcCount = 0;
			acceptanceProbSum = 0;
			lastReportTime = now;
			pathsProcessed = 0;
		}
	}
}

void PathFinder::doGenetic()
{
	const size_t kPopulationSize = 50;
	const size_t kSelectionPoolSize = 5;
	const double kMutationProbability = 0.2;

	std::vector<std::vector<Node*>> population(kPopulationSize);
	std::vector<std::vector<Node*>> nextGeneration(kPopulationSize);
	std::vector<std::vector<Node*>> selectionPool1(kSelectionPoolSize);
	std::vector<std::vector<Node*>> selectionPool2(kSelectionPoolSize);

	// Make initial population
	for (size_t i = 0; i < kPopulationSize; ++i)
		population.at(i) = getRandomPermutation(m_path);

	// Start timing
	using namespace std::chrono;
	auto lastReportTime = high_resolution_clock::now();
	unsigned long long generationCount = 0;

	while (!m_stopped) {
		// Loop population size times
		for (size_t i = 0; i < kPopulationSize; ++i) {
			// Create two selection pools
			for (size_t i = 0; i < kSelectionPoolSize; ++i) {
				selectionPool1.at(i) = *selectRandomly(population.begin(), population.end());
				selectionPool2.at(i) = *selectRandomly(population.begin(), population.end());
			}

			// Take the best one from each pool as parent 1 and parent 2 respectively
			const std::vector<Node*>& parent1 = selectBest(selectionPool1);
			const std::vector<Node*>& parent2 = selectBest(selectionPool2);

			// Perform crossover
			nextGeneration.at(i) = crossover(parent1, parent2);

			// Do random mutation on random chance
			if (randomReal() < kMutationProbability) {
				nextGeneration.at(i) = mutate(nextGeneration.at(i));
			}
		}

		// Update the initial population
		population.swap(nextGeneration);

		// Update the current best path
		std::unique_lock<std::mutex> lock(m_mutex);
		m_path = selectBest(population, &m_pathLength);
		lock.unlock();

		// Calculate current stats
		++generationCount;
		auto now = high_resolution_clock::now();
		auto timeSinceLastReport = duration_cast<microseconds>(now - lastReportTime);
		using namespace std::chrono_literals;
		if (timeSinceLastReport > 100ms) {
			m_generationsPerSecond = generationCount / (timeSinceLastReport.count() / 1000000.0);
			lastReportTime = now;
			generationCount = 0;
		}
	}
}

const std::vector<Node*>& PathFinder::selectBest(const std::vector<std::vector<Node*>>& selectionPool, double* outPathLength)
{
	size_t bestPathIdx = 0;
	double bestPathLength = calculatePathLength(selectionPool.at(bestPathIdx));
	for (size_t i = 1; i < selectionPool.size(); ++i) {
		double candidatePathLength = calculatePathLength(selectionPool.at(i));
		if (candidatePathLength < bestPathLength) {
			bestPathLength = candidatePathLength;
			bestPathIdx = i;
		}
	}

	if (outPathLength)
		*outPathLength = bestPathLength;

	return selectionPool.at(bestPathIdx);
}

std::vector<Node*> PathFinder::crossover(const std::vector<Node*>& parent1, const std::vector<Node*>& parent2)
{
	assert(parent1.size() == parent2.size());

	size_t cutPoint = randomInt(static_cast<size_t>(0), parent1.size());
	std::vector<Node*> childPath(parent1.size());

	// Take genes from parent 1
	for (size_t i = 0; i < cutPoint; ++i) {
		childPath.at(i) = parent1.at(i);
	}

	// Take genes from parent 2
	for (size_t i = cutPoint; i < parent2.size(); ++i) {
		// Make sure we don't take duplicate nodes
		auto curChildPathEnd = std::next(childPath.begin(), i);
		if (notIn(childPath.begin(), curChildPathEnd, parent2.at(i)))
			childPath.at(i) = parent2.at(i);
		else if (notIn(childPath.begin(), curChildPathEnd, parent1.at(i)))
			childPath.at(i) = parent1.at(i);
		else {
			// If we couldn't take either of the parents nodes at this position,
			// then walk backwards over both parents and select the first unused
			// node.
			for (size_t j = i - 1; j >= 0; --j) {
				if (notIn(childPath.begin(), curChildPathEnd, parent2.at(j))) {
					childPath.at(i) = parent2.at(j);
					break;
				}
				if (notIn(childPath.begin(), curChildPathEnd, parent1.at(j))) {
					childPath.at(i) = parent1.at(j);
					break;
				}
			}
		}
	}

	return childPath;
}

std::vector<Node*>& PathFinder::mutate(std::vector<Node*>& path)
{
	const double kMutationSelectionProbability = 0.5;
	auto it1 = selectRandomly(path.begin(), path.end());
	auto it2 = selectRandomly(path.begin(), path.end());
	if (randomReal() < kMutationSelectionProbability) {
		// Mutate by swapping two nodes in the path
		std::swap(*it1, *it2);
	} else {
		// Mutate by selecting two nodes in the path and 
		// reversing the order of all nodes between them
		auto it1Idx = std::distance(path.begin(), it1);
		auto it2Idx = std::distance(path.begin(), it2);
		if (it1Idx < it2Idx)
			std::reverse(it1, std::next(it2));
		else
			std::reverse(it2, std::next(it1));
	}

	return path;
}

void PathFinder::calculatePathAsync()
{
	stop();

	m_stopped = false;
	m_processingThread = std::thread(std::bind(&PathFinder::calculatePath, this));
}

void PathFinder::setMode(Mode mode)
{
	bool restart = stop();

	m_mode = mode;

	if (restart)
		calculatePathAsync();
}

void PathFinder::setTemperatureDecay(double tempDecay)
{
	m_tempDecay = tempDecay;
}

double PathFinder::getTemperatureDecay()
{
	return m_tempDecay;
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

	m_pathsPerSecond = 0;

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
	Eigen::Vector2f start = (nodeFrom.getFloatPos() + nodeFrom.getFloatSize() / 2).cast<float>();
	nvgMoveTo(ctx, start.x(), start.y());
	// Draw to center of connected node
	Eigen::Vector2f end = (nodeTo.getFloatPos() + nodeTo.getFloatSize() / 2).cast<float>();
	nvgLineTo(ctx, end.x(), end.y());
	nvgStroke(ctx);
}

void PathFinder::draw(NVGcontext* ctx)
{
	std::unique_lock<std::mutex> lock{ m_mutex };
	for (size_t i = 0; i < m_path.size(); ++i) {
		Node& nodeFrom = *m_path.at(i);
		Node& nodeTo = *m_path.at((i + 1) % m_path.size());
		drawGraphSegment(ctx, nodeFrom, nodeTo, nvgRGBA(255, 255, 255, 255));
	}
	lock.unlock();

	// Draw stats
	nvgFontFace(ctx, "sans");
	nvgFontSize(ctx, 24);
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	std::string distText = "Distance: " + toString(m_pathLength);
	nvgText(ctx, 10, 10, distText.c_str(), nullptr);
	if (m_mode != Genetic)
		nvgText(ctx, 10, 40, ("Paths Per Second: " + toString(m_pathsPerSecond)).c_str(), nullptr);
	else
		nvgText(ctx, 10, 40, ("Generations Per Second: " + toString(m_generationsPerSecond)).c_str(), nullptr);
	if (m_mode == Anealing) {
		nvgText(ctx, 10, 70, ("Temperature: " + toString(m_temperature)).c_str(), nullptr);
		nvgText(ctx, 10, 100, ("Avg Acceptance Prob: " + toString(m_avgAcceptanceProb)).c_str(), nullptr);
		nvgText(ctx, 10, 130, ("Temperature Decay: " + toString(m_tempDecay * 100) + "% Per Second").c_str(), nullptr);
	}
}

double PathFinder::euclideanDistSquared(const Node& nodeSrc, const Node& nodeDst)
{
	Eigen::Vector2d displacement = nodeDst.getFloatPos() - nodeSrc.getFloatPos();
	return displacement.dot(displacement);
}

double PathFinder::euclideanDist(const Node& nodeSrc, const Node& nodeDst)
{
	return std::sqrt(euclideanDistSquared(nodeSrc, nodeDst));
}

double PathFinder::calculatePathLength(const std::vector<Node*>& traversalList)
{
	double accumDist = 0;
	for (size_t i = 0; i < traversalList.size(); ++i) {
		Node& nodeFrom = *traversalList[i];
		Node& nodeto = *traversalList[(i + 1) % traversalList.size()];
		accumDist += euclideanDist(nodeFrom, nodeto);
	}

	return accumDist;
}

double PathFinder::calculateAcceptanceProbability(double candidatePathLength)
{
	// 100% acceptance for paths that are better
	if (candidatePathLength < m_pathLength)
		return 1;

	// Skip paths that are the same, or always skip if worse in hill climbing mode
	if (candidatePathLength == m_pathLength || m_mode == HillClimbing)
		return 0;

	double exponent = (m_pathLength - candidatePathLength) / m_temperature;
	if (exponent != -std::numeric_limits<double>::infinity()) {
		return std::exp(exponent);
	}

	return 0;
}
