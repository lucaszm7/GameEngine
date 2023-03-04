#pragma once

// #define DLLExport __declspec(dllexport)

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

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

	std::vector<Eigen::Vector3f> controlPoints;
	std::vector<std::vector<Eigen::Vector3f>> controlPointsVectorDir;
	std::vector<std::vector<Eigen::Vector3f>> controlPointsVectorPos;

	float uniformRadius{ 1.0f };
};

