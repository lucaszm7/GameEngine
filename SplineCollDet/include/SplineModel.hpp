#pragma once

// #define DLLExport __declspec(dllexport)

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <numbers>

#include <GLM/glm.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

struct SplineModel
{
	SplineModel(const std::vector<Eigen::Vector3f>& controlPoints, 
				const std::vector<std::vector<Eigen::Vector3f>>& controlPointsVectorDir);

	SplineModel(const float* controlPoints,
				const float* controlPointsVectorDir,
				unsigned int controlPointsCount,
				unsigned int vectorsPerControlPointsCount);

	SplineModel(Eigen::Vector3f posInit,
				Eigen::Vector3f posEnd,
				const int controlPointsCount,
				const int vectorsPerControlPointCount,
				const float radius);

	std::vector<Eigen::Vector3f> controlPoints;
	std::vector<std::vector<Eigen::Vector3f>> controlPointsVectorDir;
	std::vector<std::vector<Eigen::Vector3f>> controlPointsVectorPos;

	float uniformRadius{ 1.0f };
};

