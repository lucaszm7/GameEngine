#include "Spline.hpp"

Spline::Spline(const std::string& filePath)
{
	this->name = filePath.substr(filePath.find_last_of("/"), filePath.size() - 1);
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

		this->m_controlPoints.reserve(controlPoints);
		this->m_controlPointsVectorDir.resize(controlPoints);
		this->m_controlPointsVectorPos.resize(controlPoints);

		for (unsigned int i = 0; i < controlPoints; ++i)
		{
			float x, y, z;
			std::getline(file, line);
			std::istringstream issControlPoint(line);
			issControlPoint >> x >> y >> z;
			this->m_controlPoints.emplace_back(glm::vec3(x * scalerFactor, z * scalerFactor, y * scalerFactor));
			this->m_controlPointsVectorDir[i].reserve(vectorsPerControlPoint + 1);
			this->m_controlPointsVectorPos[i].reserve(vectorsPerControlPoint + 1);

			for (unsigned int j = 0; j < vectorsPerControlPoint; ++j)
			{
				std::getline(file, line);
				std::istringstream issControlRadii(line);
				issControlRadii >> x >> y >> z;
				this->m_controlPointsVectorDir[i].emplace_back(x * scalerFactor, z * scalerFactor, y * scalerFactor);
				this->m_controlPointsVectorPos[i].push_back(this->m_controlPoints.back() + this->m_controlPointsVectorDir[i].back());
			}
		}

		file.close();
		TransformFromGlmToEigen();
		std::cout << "INFO: colon spline readed:\n";
	}
	else
	{
		std::cout << "Error reading control points: " << filePath << std::endl;
	}
}

Spline::Spline(const Spline& otherSpline)
	: m_controlPoints(otherSpline.m_controlPoints), 
	  m_controlPointsVectorDir(otherSpline.m_controlPointsVectorDir),
	  m_controlPointsVectorPos(otherSpline.m_controlPointsVectorPos)
{
	TransformFromGlmToEigen();
}

void Spline::Draw(Shader& shader)
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

void Spline::TransformPoints()
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

void Spline::TransformFromGlmToEigen()
{
	splineModel->controlPoints.clear();
	splineModel->controlPoints.reserve(m_controlPoints.size());
	for (const auto& controlPoint : m_controlPoints)
	{
		splineModel->controlPoints.emplace_back(controlPoint.x, controlPoint.y, controlPoint.z);
	}

	splineModel->controlPointsVectorDir.clear();
	splineModel->controlPointsVectorDir.reserve(m_controlPointsVectorDir.size());
	for (const auto& vectorDir : m_controlPointsVectorDir)
	{
		splineModel->controlPointsVectorDir.emplace_back();
		splineModel->controlPointsVectorDir.back().reserve(vectorDir.size());
		for (const auto& vector : vectorDir)
		{
			splineModel->controlPointsVectorDir.back().emplace_back(vector.x, vector.y, vector.z);
		}
	}

	splineModel->controlPointsVectorPos.clear();
	splineModel->controlPointsVectorPos.reserve(m_controlPointsVectorPos.size());
	for (const auto& vectorPos : m_controlPointsVectorPos)
	{
		splineModel->controlPointsVectorPos.emplace_back();
		splineModel->controlPointsVectorPos.back().reserve(vectorPos.size());
		for (const auto& vector : vectorPos)
		{
			splineModel->controlPointsVectorPos.back().emplace_back(vector.x, vector.y, vector.z);
		}
	}
}

void Spline::GenerateSplineMesh(const std::string& texPath, TriangleOrientation triangleOrientation)
{
	const unsigned int numControlPoints = m_controlPoints.size();
	const unsigned int numControlRadii = m_controlPointsVectorDir[0].size();

	float duv_x = 1.0f / (numControlRadii - 1.0f);
	float duv_y = 1.0f / (numControlPoints - 1.0f);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	m_controlPointsVectorPos.clear();
	m_controlPointsVectorPos.resize(m_controlPoints.size());
	// Calc all vertices
	for (unsigned int i = 0; i < numControlPoints; ++i)
	{
		for (int j = 0; j < numControlRadii + 1; ++j)
		{
			// 1 Vertex
			Vertex v;
			//v.normal = spline.controlPointsVectorDir[i][j]; // 0.05f circle
			if (j < numControlRadii)
				v.Normal = m_controlPointsVectorDir[i][j]; // 0.05f * radius;
			else
				v.Normal = m_controlPointsVectorDir[i][0]; // 0.05f * radius;

			//std::cout << v.normal << std::endl;
			//std::cout << spline.controlPoints[i] << std::endl;
			v.Position = v.Normal + m_controlPoints[i];
			v.Normal = glm::normalize(v.Normal);
			v.TexCoord = glm::vec2(duv_x * j, duv_y * i);

			vertices.push_back(v);
			m_controlPointsVectorPos[i].push_back(v.Position);
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

			if (triangleOrientation == TriangleOrientation::CounterClockWise)
			{
				indices.push_back(i1);
				indices.push_back(i4);
				indices.push_back(i3);
				indices.push_back(i1);
				indices.push_back(i3);
				indices.push_back(i2);
			}
			else // triangleOrientarion == ClockWise
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
	mesh.SetupMesh(vertices, indices, textures);
}

void GenerateSurface(Spline& spline, const std::string& texPath)
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

void GenerateEndoscope(Spline& spline, 
					   Eigen::Vector3f posInit, 
					   Eigen::Vector3f posEnd, 
					   const int controlPoints, 
					   const int vectorsPerControlPoint, 
					   const float radius) 
{
	spline.name = "Endoscope " + std::to_string(controlPoints);
	spline.splineModel->uniformRadius = radius;

	Eigen::Vector3f ptop = posEnd - posInit;
	Eigen::Vector3f ptopn = ptop / (float)(controlPoints - 1);
	spline.m_controlPoints.reserve(controlPoints);
	spline.m_controlPointsVectorDir.resize(controlPoints);
	spline.m_controlPointsVectorPos.resize(controlPoints);
	Eigen::Vector3f pci;
	pci = posInit;

	for (int i = 0; i < controlPoints; ++i)
	{
		auto point = pci + ptopn * (float)i;
		spline.m_controlPoints.emplace_back(point.x(), point.y(), point.z());
		spline.m_controlPointsVectorDir[i].reserve(vectorsPerControlPoint);
		spline.m_controlPointsVectorPos[i].reserve(vectorsPerControlPoint);
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
			spline.m_controlPointsVectorDir[i].emplace_back(ortho.x(), ortho.y(), ortho.z());
			spline.m_controlPointsVectorPos[i].emplace_back(spline.m_controlPoints[i] + spline.m_controlPointsVectorDir[i].back());
			ortho = qr * ortho;
		}
	}

	spline.TransformFromGlmToEigen();
}


