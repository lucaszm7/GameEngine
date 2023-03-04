#include <string>
#include <vector>
#include <memory>

#include "mesh.h"
#include "SplineModel.hpp"

enum class TriangleOrientation
{
	ClockWise,
	CounterClockWise
};

// Layer of abstraction of SplineModel to Demo
struct Spline
{
	explicit Spline(const std::string& filePath);
	Spline(const Spline& otherSpline);

	std::vector<glm::vec3> m_controlPoints;
	std::vector<std::vector<glm::vec3>> m_controlPointsVectorDir;
	std::vector<std::vector<glm::vec3>> m_controlPointsVectorPos;

	std::unique_ptr<SplineModel> splineModel;

	Mesh mesh;
	Transform transform;
	std::string name;

	void Draw(Shader& shader);
	void TransformPoints();
	void TransformFromGlmToEigen();
	void GenerateSplineMesh(const std::string& texPath, TriangleOrientation triangleOrientation);
};

void GenerateSurface(Spline& spline, const std::string& texPath);
void GenerateEndoscope(Spline& splineModel, Eigen::Vector3f posInit, Eigen::Vector3f posEnd,
	const int controlPoints, const int vectorsPerControlPoint, const float radius);
