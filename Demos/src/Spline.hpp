#include <string>
#include <vector>
#include <memory>

#include "mesh.h"
#include "SplineModel.hpp"
#include <IMGUI/imgui.h>



// Layer of abstraction of SplineModel to Demo
struct Spline
{
	explicit Spline(const std::string& filePath);

	Spline(const Spline&) = delete;
	Spline& operator= (const Spline&) = delete;

	std::shared_ptr<SplineModel> splineModel;

	std::vector<glm::vec3> m_controlPoints;
	std::vector<std::vector<glm::vec3>> m_controlPointsVectorDir;
	std::vector<std::vector<glm::vec3>> m_controlPointsVectorPos;

	Transform transform;
	Mesh mesh;
	std::string name;

	void Draw(Shader& shader);
	void OnImGui();
	void GenerateSplineMesh(const std::string& texPath, TriangleOrientation triangleOrientation);
	std::shared_ptr<SplineModel> GetSplineModelTransform();
};
