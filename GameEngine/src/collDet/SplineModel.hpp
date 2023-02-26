#pragma once

#include <vector>
#include <string>

#include <GLM/glm.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "mesh.h";

struct SplineModel
{
	SplineModel() = default;
	std::vector<glm::vec3> m_controlPoints;
	std::vector<std::vector<glm::vec3>> m_controlPointsVectorDir;
	std::vector<std::vector<glm::vec3>> m_controlPointsVectorPos;

	std::vector<Eigen::Vector3f> controlPoints;
	std::vector<std::vector<Eigen::Vector3f>> controlPointsVectorDir;
	std::vector<std::vector<Eigen::Vector3f>> controlPointsVectorPos;

	float uniformRadius{ 1.0f };
	Mesh mesh;
	Transform transform;
	std::string name;

	void Draw(Shader& shader);
	void TransformPoints();
	void TransformFromGlmToEigen();
};

void TransformFromGlmToEigen(SplineModel& splineModel);
void LoadSplineModel(const std::string& filePath, SplineModel& splineModel);
void GenerateSplineMesh(SplineModel& spline, const std::string& texPath, bool CCW = true);
void GenerateSurface(SplineModel& spline, const std::string& texPath);
void GenerateEndoscope(SplineModel& splineModel, Eigen::Vector3f posInit, Eigen::Vector3f posEnd,
	const int controlPoints, const int vectorsPerControlPoint, const float radius);
