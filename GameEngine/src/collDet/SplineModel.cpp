#include "collDet/SplineModel.hpp"

void SplineModel::Draw(Shader& shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, transform.scale);
	shader.SetUniformMatrix4fv("model", model);

	mesh.Draw(shader);
}

void SplineModel::TransformPoints()
{
	for (auto& controlPoint : m_controlPoints)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		controlPoint = glm::vec3(model * glm::vec4(controlPoint, 1.0));
	}
	for (auto& controlPointsVectorPos : m_controlPointsVectorPos)
	{
		for (auto& pointsVectorPos : controlPointsVectorPos)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, transform.position);
			pointsVectorPos = glm::vec3(model * glm::vec4(pointsVectorPos, 1.0));
		}
	}
	TransformFromGlmToEigen();
}

void SplineModel::TransformFromGlmToEigen()
{
	controlPoints.clear();
	controlPoints.reserve(m_controlPoints.size());
	for (const auto& controlPoint : m_controlPoints)
	{
		controlPoints.emplace_back(controlPoint.x, controlPoint.y, controlPoint.z);
	}

	controlPointsVectorDir.clear();
	controlPointsVectorDir.reserve(m_controlPointsVectorDir.size());
	for (const auto& vectorDir : m_controlPointsVectorDir)
	{
		controlPointsVectorDir.push_back(std::vector<Eigen::Vector3f>());
		controlPointsVectorDir.back().reserve(vectorDir.size());
		for (const auto& vector : vectorDir)
		{
			controlPointsVectorDir.back().emplace_back(vector.x, vector.y, vector.z);
		}
	}

	controlPointsVectorPos.clear();
	controlPointsVectorPos.reserve(m_controlPointsVectorPos.size());
	for (const auto& vectorPos : m_controlPointsVectorPos)
	{
		controlPointsVectorPos.push_back(std::vector<Eigen::Vector3f>());
		controlPointsVectorPos.back().reserve(vectorPos.size());
		for (const auto& vector : vectorPos)
		{
			controlPointsVectorPos.back().emplace_back(vector.x, vector.y, vector.z);
		}
	}
}

void TransformFromGlmToEigen(SplineModel& splineModel)
{
	splineModel.controlPoints.clear();
	splineModel.controlPoints.reserve(splineModel.m_controlPoints.size());
	for (const auto& controlPoint : splineModel.m_controlPoints)
	{
		splineModel.controlPoints.emplace_back(controlPoint.x, controlPoint.y, controlPoint.z);
	}

	splineModel.controlPointsVectorDir.clear();
	splineModel.controlPointsVectorDir.reserve(splineModel.m_controlPointsVectorDir.size());
	for (const auto& vectorDir : splineModel.m_controlPointsVectorDir)
	{
		splineModel.controlPointsVectorDir.push_back(std::vector<Eigen::Vector3f>());
		splineModel.controlPointsVectorDir.back().reserve(vectorDir.size());
		for (const auto& vector : vectorDir)
		{
			splineModel.controlPointsVectorDir.back().emplace_back(vector.x, vector.y, vector.z);
		}
	}

	splineModel.controlPointsVectorPos.clear();
	splineModel.controlPointsVectorPos.reserve(splineModel.m_controlPointsVectorPos.size());
	for (const auto& vectorPos : splineModel.m_controlPointsVectorPos)
	{
		splineModel.controlPointsVectorPos.push_back(std::vector<Eigen::Vector3f>());
		splineModel.controlPointsVectorPos.back().reserve(vectorPos.size());
		for (const auto& vector : vectorPos)
		{
			splineModel.controlPointsVectorPos.back().emplace_back(vector.x, vector.y, vector.z);
		}
	}
}

void LoadSplineModel(const std::string& filePath, SplineModel& splineModel)
{
	splineModel.name = filePath.substr(filePath.find_last_of("/"), filePath.size() - 1);
	float scalerFactor = 100.0f;
	std::ifstream file(filePath);
	if (file.is_open())
	{
		std::string line;
		unsigned int controlPoints = 0;
		unsigned int vectorsPerControlPoint = 0;
		std::getline(file, line);
		std::istringstream iss(line);
		iss >> controlPoints >> vectorsPerControlPoint;
		std::cout << controlPoints << std::endl;
		std::cout << vectorsPerControlPoint << std::endl;

		splineModel.m_controlPoints.reserve(controlPoints);
		splineModel.m_controlPointsVectorDir.resize(controlPoints);
		splineModel.m_controlPointsVectorPos.resize(controlPoints);

		for (unsigned int i = 0; i < controlPoints; ++i)
		{
			float x, y, z;
			std::getline(file, line);
			std::istringstream issControlPoint(line);
			issControlPoint >> x >> y >> z;
			splineModel.m_controlPoints.emplace_back(glm::vec3(x * scalerFactor, z * scalerFactor, y * scalerFactor));
			splineModel.m_controlPointsVectorDir[i].reserve(vectorsPerControlPoint + 1);
			splineModel.m_controlPointsVectorPos[i].reserve(vectorsPerControlPoint + 1);

			for (unsigned int j = 0; j < vectorsPerControlPoint; ++j)
			{
				std::getline(file, line);
				std::istringstream issControlRadii(line);
				issControlRadii >> x >> y >> z;
				splineModel.m_controlPointsVectorDir[i].emplace_back(x * scalerFactor, z * scalerFactor, y * scalerFactor);
				splineModel.m_controlPointsVectorPos[i].push_back(splineModel.m_controlPoints.back() + splineModel.m_controlPointsVectorDir[i].back());
			}
		}

		file.close();
		TransformFromGlmToEigen(splineModel);
		std::cout << "INFO: colon spline readed:\n";
	}
	else
	{
		std::cout << "Error reading control points: " << filePath << std::endl;
	}
}

void GenerateSplineMesh(SplineModel& spline, const std::string& texPath, bool CCW)
{
	const unsigned int numControlPoints = spline.m_controlPoints.size();
	const unsigned int numControlRadii = spline.m_controlPointsVectorDir[0].size();

	float duv_x = 1.0f / (numControlRadii - 1.0f);
	float duv_y = 1.0f / (numControlPoints - 1.0f);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	spline.m_controlPointsVectorPos.clear();
	spline.m_controlPointsVectorPos.resize(spline.m_controlPoints.size());
	// Calc all vertices
	for (unsigned int i = 0; i < numControlPoints; ++i)
	{
		for (int j = 0; j < numControlRadii + 1; ++j)
		{
			// 1 Vertex
			Vertex v;
			//v.normal = spline.controlPointsVectorDir[i][j]; // 0.05f circle
			if (j < numControlRadii)
				v.Normal = spline.m_controlPointsVectorDir[i][j]; // 0.05f * radius;
			else
				v.Normal = spline.m_controlPointsVectorDir[i][0]; // 0.05f * radius;

			//std::cout << v.normal << std::endl;
			//std::cout << spline.controlPoints[i] << std::endl;
			v.Position = v.Normal + spline.m_controlPoints[i];
			v.Normal = glm::normalize(v.Normal);
			v.TexCoord = glm::vec2(duv_x * j, duv_y * i);

			vertices.push_back(v);
			spline.m_controlPointsVectorPos[i].push_back(v.Position);
		}
	}

	const unsigned int numControlRadiiInc = numControlRadii + 1;
	// Calc indices as quads
	for (unsigned int i = 1; i < numControlPoints; ++i)
	{
		for (unsigned int j = 1; j < numControlRadiiInc; ++j)
		{
			// P4--P2
			// | \ |
			// |  \|
			// P3--P1 
			//
			// 4 Indices
			unsigned int i1, i2, i3, i4;
			i1 = i * numControlRadiiInc + j;
			i2 = i * numControlRadiiInc + (j - 1);
			i3 = (i - 1) * numControlRadiiInc + (j - 1);
			i4 = (i - 1) * numControlRadiiInc + j;

			if (CCW)
			{
				indices.push_back(i1);
				indices.push_back(i4);
				indices.push_back(i3);
				indices.push_back(i1);
				indices.push_back(i3);
				indices.push_back(i2);
			}
			else
			{
				indices.push_back(i1);
				indices.push_back(i2);
				indices.push_back(i3);
				indices.push_back(i1);
				indices.push_back(i3);
				indices.push_back(i4);
			}
		}
	}

	std::vector<Texture> textures;
	Texture tex(texPath, Texture::Type::DIFFUSE, Texture::Parameter::REPEAT);
	textures.push_back(tex);
	spline.mesh.SetupMesh(vertices, indices, textures);
}

void GenerateSurface(SplineModel& spline, const std::string& texPath)
{
	Mesh mesh;
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;

	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	glm::vec3 tempVert(0);
	glm::vec3 tempVertNormalized(0);

	unsigned int controlPointsCount = spline.m_controlPoints.size();
	unsigned int vectorsCount = spline.m_controlPointsVectorDir[0].size();

	for (int i = 0; i < controlPointsCount; i++)
	{
		tempVert = spline.m_controlPointsVectorPos[i][0];
		tempVertNormalized = glm::normalize(spline.m_controlPointsVectorDir[i][0]);
		for (int j = 0; j < vectorsCount - 1; j++)
		{
			Vertex vertex;
			vertex.Position = spline.m_controlPointsVectorPos[i][j];
			vertex.TexCoord = glm::vec2(j / vectorsCount, i / (vectorsCount));
			vertex.Normal = glm::normalize(spline.m_controlPointsVectorDir[i][j]);
			vertices.push_back(vertex);

			if (j == vectorsCount - 1)
			{
				Vertex tempVertex;
				tempVertex.Position = tempVert;
				tempVertex.TexCoord = glm::vec2((j + 1) / vectorsCount, i / (vectorsCount * 0.5f));
				tempVertex.Normal = tempVertNormalized;
				vertices.push_back(tempVertex);
				spline.m_controlPointsVectorPos[i][j + 1] = tempVert;
			}
		}

	}

	vectorsCount++;
	int v1, v2, v3, v4;
	for (int i = 0; i < controlPointsCount - 1; i++)
	{
		for (int j = 0; j < vectorsCount; j++)
		{
			// P4--P3
			// |  /|
			// | / |
			// P2--P1
			// 2 - 3 - 1
			// 4 - 3 - 2

			if (j < vectorsCount - 1)
			{
				v1 = i * vectorsCount + j;
				v2 = i * vectorsCount + (j + 1);
				v3 = (i + 1) * vectorsCount + j;
				v4 = (i + 1) * vectorsCount + (j + 1);

				indices.push_back(v2);
				indices.push_back(v3);
				indices.push_back(v1);
				indices.push_back(v4);
				indices.push_back(v3);
				indices.push_back(v2);
			}
			else
			{
				v1 = i * vectorsCount + (vectorsCount - 1);
				v2 = i * vectorsCount + 0;
				v3 = (i + 1) * vectorsCount + (vectorsCount - 1);
				v4 = (i + 1) * vectorsCount + 0;
				indices.push_back(v2);
				indices.push_back(v3);
				indices.push_back(v1);
				indices.push_back(v4);
				indices.push_back(v3);
				indices.push_back(v2);
			}
		}
	}

	std::vector<Texture> textures;
	Texture tex(texPath, Texture::Type::DIFFUSE, Texture::Parameter::REPEAT);
	textures.push_back(tex);
	spline.mesh.SetupMesh(vertices, indices, textures);
}

void GenerateEndoscope(SplineModel& splineModel, Eigen::Vector3f posInit, Eigen::Vector3f posEnd, const int controlPoints, const int vectorsPerControlPoint, const float radius) {
	splineModel.name = "Endoscope " + std::to_string(controlPoints);
	splineModel.uniformRadius = radius;

	Eigen::Vector3f ptop = posEnd - posInit;
	Eigen::Vector3f ptopn = ptop / (float)(controlPoints - 1);
	splineModel.m_controlPoints.reserve(controlPoints);
	splineModel.m_controlPointsVectorDir.resize(controlPoints);
	splineModel.m_controlPointsVectorPos.resize(controlPoints);
	Eigen::Vector3f pci;
	pci = posInit;

	for (int i = 0; i < controlPoints; ++i)
	{
		auto point = pci + ptopn * (float)i;
		splineModel.m_controlPoints.emplace_back(point.x(), point.y(), point.z());
		splineModel.m_controlPointsVectorDir[i].reserve(vectorsPerControlPoint);
		splineModel.m_controlPointsVectorPos[i].reserve(vectorsPerControlPoint);
	}

	for (int i = 0; i < controlPoints; ++i) {
		Eigen::Vector3f w = ptop; // rotation axis
		w.normalize();
		Eigen::Vector3f ortho = ptopn;
		if (ortho.y() == 0.0f && ortho.z() == 0.0f)
			ortho = ortho.cross(Eigen::Vector3f::UnitY()).normalized();
		else
			ortho = ortho.cross(Eigen::Vector3f::UnitX()).normalized();

		ortho *= radius;
		constexpr float my_pi = float(3.14159265358979323846);
		constexpr float degToRad = my_pi / float(180);
		Eigen::Quaternionf qr(Eigen::AngleAxisf(-2.0f * my_pi / (float)vectorsPerControlPoint, w));
		for (int j = 0; j < vectorsPerControlPoint; ++j)
		{
			splineModel.m_controlPointsVectorDir[i].emplace_back(ortho.x(), ortho.y(), ortho.z());
			splineModel.m_controlPointsVectorPos[i].emplace_back(splineModel.m_controlPoints[i] + splineModel.m_controlPointsVectorDir[i].back());
			ortho = qr * ortho;
		}
	}

	TransformFromGlmToEigen(splineModel);
}

