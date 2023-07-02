#pragma once

#include <cmath>
#include <iostream>
#include <array>

#include <limits>
#include <memory>
#include <cstdlib>
#include <random>
#include <numbers>

using std::sqrt;

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = std::numbers::pi;

namespace cgl
{
	inline double random_double()
	{
		// Returns a number between [0, 1).
		return rand() / (RAND_MAX + 1.0);
	}

	inline double random_double(double min, double max)
	{
		// Returns a number between [min, max).
		return min + (max - min) * random_double();
	}

	inline double clamp(double x, double min, double max)
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	inline double degree_to_radians(double degree)
	{
		return degree * pi / 180.0;
	}
}