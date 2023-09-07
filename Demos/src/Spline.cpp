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

		std::vector<float> pControlPoints;
		std::vector<float> pControlPointsVecDir;

		pControlPoints.reserve(controlPoints);
		pControlPointsVecDir.reserve(controlPoints * (vectorsPerControlPoint + 1));

		for (unsigned int i = 0; i < controlPoints; ++i)
		{
			float x, y, z;
			std::getline(file, line);
			std::istringstream issControlPoint(line);
			issControlPoint >> x >> y >> z;
			this->m_controlPoints.emplace_back(glm::vec3(x * scalerFactor, z * scalerFactor, y * scalerFactor));
			this->m_controlPointsVectorDir[i].reserve(vectorsPerControlPoint + 1);
			this->m_controlPointsVectorPos[i].reserve(vectorsPerControlPoint + 1);

			pControlPoints.emplace_back(x * scalerFactor);
			pControlPoints.emplace_back(y * scalerFactor);
			pControlPoints.emplace_back(z * scalerFactor);

			for (unsigned int j = 0; j < vectorsPerControlPoint; ++j)
			{
				std::getline(file, line);
				std::istringstream issControlRadii(line);
				issControlRadii >> x >> y >> z;
				this->m_controlPointsVectorDir[i].emplace_back(x * scalerFactor, z * scalerFactor, y * scalerFactor);
				this->m_controlPointsVectorPos[i].push_back(this->m_controlPoints.back() + this->m_controlPointsVectorDir[i].back());
			
				pControlPointsVecDir.emplace_back(x * scalerFactor);
				pControlPointsVecDir.emplace_back(y * scalerFactor);
				pControlPointsVecDir.emplace_back(z * scalerFactor);
			}
		}

		file.close();
		unsigned int nControlPointsCount = m_controlPoints.size();
		unsigned int nVectorsPerControlPointCount = m_controlPointsVectorDir.front().size();
		this->splineModel = std::make_shared<SplineModel>((float*)pControlPoints.data(), (float*)pControlPointsVecDir.data(),
														  nControlPointsCount, nVectorsPerControlPointCount);
		std::cout << "INFO: colon spline readed:\n";
	}
	else
	{
		std::cout << "Error reading control points: " << filePath << std::endl;
	}
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

void Spline::OnImGui()
{
	if (ImGui::TreeNode(("Transform" + name).c_str()))
	{
		ImGui::DragFloat3("Position:", &transform.position[0], 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3("Rotation:", &transform.rotation[0], 0.1f, -glm::pi<float>(), glm::pi<float>());
		ImGui::DragFloat3("Scale:", &transform.scale[0], 0.01f, -10.0f, 10.0f);
		ImGui::TreePop();
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

	std::vector<std::shared_ptr<Texture>> textures;
	std::shared_ptr<Texture> tex;
	tex = std::make_shared<Texture>(texPath, Texture::Type::DIFFUSE, Texture::Wrap::REPEAT);
	textures.push_back(tex);
	mesh.SetupMesh(vertices, indices, textures);
}

std::shared_ptr<SplineModel> Spline::GetSplineModelTransform()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	glm::vec3 temp;

	for (unsigned int i = 0; i < m_controlPoints.size(); ++i)
	{
		temp = glm::vec3(model * glm::vec4(m_controlPoints[i], 1.0));
		splineModel->controlPoints[i] = Eigen::Vector3f(temp.x, temp.y, temp.z);

		for (unsigned int j = 0; j < m_controlPointsVectorPos[i].size(); ++j)
		{
			temp = glm::vec3(model * glm::vec4(m_controlPointsVectorPos[i][j], 1.0));
			splineModel->controlPointsVectorPos[i][j] = Eigen::Vector3f(temp.x, temp.y, temp.z);
		}
	}

	return splineModel;
}

