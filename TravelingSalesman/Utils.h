#pragma once

#include <array>
#include  <random>
#include  <iterator>
#include <sstream>
#include <iomanip>

template <typename T, size_t DimFirst, size_t... Dims>
class NDArray : public std::array<NDArray<T, Dims...>, DimFirst> {};

template <typename T, size_t DimLast>
class NDArray<T, DimLast> : public std::array<T, DimLast> {};

template <typename Vector>
typename Vector::iterator unorderedErase(Vector& v, typename Vector::iterator it) {
	if (it == std::prev(v.end()))
		it = v.end();
	else
		*it = std::move(v.back());
	v.pop_back();
	return it;
}

template <typename Vector>
void unorderedErase(Vector& v, size_t i) {
	if (i != v.size() - 1)
		v.at(i) = std::move(v.back());
	v.pop_back();
}

template<typename Iter, typename RandomGenerator>
Iter selectRandomly(Iter start, Iter end, RandomGenerator& g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter selectRandomly(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return selectRandomly(start, end, gen);
}

template <typename T>
std::string toString(const T value, const int decimalPlaces = 2)
{
	std::ostringstream out;
	out << std::fixed;
	out << std::setprecision(decimalPlaces) << value;
	return out.str();
}