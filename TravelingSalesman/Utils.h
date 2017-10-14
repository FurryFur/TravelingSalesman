//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A header only collection of useful utility functions and types.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <array>
#include <random>
#include <iterator>
#include <sstream>
#include <iomanip>

// A simple mulidimensional array
template <typename T, size_t DimFirst, size_t... Dims>
class NDArray : public std::array<NDArray<T, Dims...>, DimFirst> {};

// A simple mulidimensional array
template <typename T, size_t DimLast>
class NDArray<T, DimLast> : public std::array<T, DimLast> {};

// Erases an element from a vector in O(1) time without preserving order.
template <typename Vector>
typename Vector::iterator unorderedErase(Vector& v, typename Vector::iterator it) 
{
	if (it == std::prev(v.end()))
		it = v.end();
	else
		*it = std::move(v.back());
	v.pop_back();
	return it;
}

// Erases an element from a vector in O(1) time without preserving order
template <typename Vector>
void unorderedErase(Vector& v, size_t i) 
{
	if (i != v.size() - 1)
		v.at(i) = std::move(v.back());
	v.pop_back();
}

// Returns a generator for generating random numbers
std::mt19937& getRandomGenerator() 
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return gen;
}

// Returns a random real number, uniformly distributed in the range [min, max)
template<typename RealT = double>
RealT randomReal(RealT min = 0, RealT max = 1) 
{
	std::uniform_real_distribution<RealT> dist(min, max);
	return dist(getRandomGenerator());
}

// Returns a random integer, uniformly distributed in the range [min, max] inclusive
template<typename IntT = int>
IntT randomInt(IntT min = 0, IntT max = 1) 
{
	std::uniform_int_distribution<IntT> dist(min, max);
	return dist(getRandomGenerator());
}

// Returns an iterator to a random element in the range [iterStart, iterEnd] inclusive
template<typename Iter, typename RandomGenerator>
Iter selectRandomly(Iter start, Iter end, RandomGenerator& g) 
{
	std::uniform_int_distribution<std::_Iter_diff_t<Iter>> dist(0, std::distance(start, end) - 1);
	std::advance(start, dist(g));
	return start;
}

// Returns an iterator to a random element in the range [iterStart, iterEnd] inclusive
template<typename Iter>
Iter selectRandomly(Iter start, Iter end) 
{
	return selectRandomly(start, end, getRandomGenerator());
}

// Returns a random permutation of the collection
template<typename CollectionT>
CollectionT getRandomPermutation(const CollectionT& collection) {
	CollectionT randomPermutation = collection;
	for (size_t i = 0; i < collection.size(); ++i) {
		size_t selectedIdx = randomInt(static_cast<size_t>(i), collection.size() - 1);
		std::swap(randomPermutation.at(i), randomPermutation.at(selectedIdx));
	}

	return randomPermutation;
}

// Checks if the specified element exists in the range [start, end) exclusive
template<typename Iter, typename T>
bool in(Iter start, Iter end, T val) {
	auto resultIt = std::find(start, end, val);
	if (resultIt != end)
		return true;
	else
		return false;
}

// Checks if the specified element does not exists in the range [start, end) exclusive
template<typename Iter, typename T>
bool notIn(Iter start, Iter end, T val) {
	return !in(start, end, val);
}

// Converts a number to a string with the specified number of decimal places
template <typename T>
std::string toString(const T value, const int decimalPlaces = 2)
{
	std::ostringstream out;
	out << std::fixed;
	out << std::setprecision(decimalPlaces) << value;
	return out.str();
}