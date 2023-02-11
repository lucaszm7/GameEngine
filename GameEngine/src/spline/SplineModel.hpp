#pragma once

#include <vector>
#include <string>
#include <GLM/glm.hpp>
#include "mesh.h";

struct SplineModel
{
	SplineModel() = default;
	std::vector<glm::vec3> controlPoints;
	std::vector<std::vector<glm::vec3>> controlPointsVectorDir;
	std::vector<std::vector<glm::vec3>> controlPointsVectorPos;
	float uniformRadius{ 0.0f };
	Mesh mesh;
	Transform transform;

	void Draw(Shader& shader)
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

};
void LoadSplineModel(const std::string& filePath, SplineModel& splineModel)
{
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

		splineModel.controlPoints.reserve(controlPoints);
		splineModel.controlPointsVectorDir.resize(controlPoints);
		splineModel.controlPointsVectorPos.resize(controlPoints);

		for (unsigned int i = 0; i < controlPoints; ++i)
		{
			float x, y, z;
			std::getline(file, line);
			std::istringstream issControlPoint(line);
			issControlPoint >> x >> y >> z;
			splineModel.controlPoints.emplace_back(glm::vec3(x * scalerFactor, z * scalerFactor, y * scalerFactor));
			splineModel.controlPointsVectorDir[i].reserve(vectorsPerControlPoint + 1);
			splineModel.controlPointsVectorPos[i].reserve(vectorsPerControlPoint + 1);

			for (unsigned int j = 0; j < vectorsPerControlPoint; ++j)
			{
				std::getline(file, line);
				std::istringstream issControlRadii(line);
				issControlRadii >> x >> y >> z;
				splineModel.controlPointsVectorDir[i].emplace_back(x * scalerFactor, z * scalerFactor, y * scalerFactor);
				splineModel.controlPointsVectorPos[i].push_back(splineModel.controlPoints.back() + splineModel.controlPointsVectorDir[i].back());
			}
		}

		file.close();
		std::cout << "INFO: colon spline readed:\n";
	}
	else
	{
		std::cout << "Error reading control points: " << filePath << std::endl;
	}
}

void GenerateSplineMesh(SplineModel& spline)
{
	const unsigned int numControlPoints = spline.controlPoints.size();
	const unsigned int numControlRadii = spline.controlPointsVectorDir[0].size();

	float duv_x = 1.0f / (numControlRadii - 1.0f);
	float duv_y = 1.0f / (numControlPoints - 1.0f);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	spline.controlPointsVectorPos.clear();
	spline.controlPointsVectorPos.resize(spline.controlPoints.size());
	// Calc all vertices
	for (unsigned int i = 0; i < numControlPoints; ++i)
	{
		for (int j = 0; j < numControlRadii + 1; ++j)
		{
			// 1 Vertex
			Vertex v;
			//v.normal = spline.controlPointsVectorDir[i][j]; // 0.05f circle
			if (j < numControlRadii)
				v.Normal = spline.controlPointsVectorDir[i][j]; // 0.05f * radius;
			else
				v.Normal = spline.controlPointsVectorDir[i][0]; // 0.05f * radius;

			//std::cout << v.normal << std::endl;
			//std::cout << spline.controlPoints[i] << std::endl;
			v.Position = v.Normal + spline.controlPoints[i];
			v.Normal = glm::normalize(v.Normal);
			v.TexCoord = glm::vec2(duv_x * j, duv_y * i);

			vertices.push_back(v);
			spline.controlPointsVectorPos[i].push_back(v.Position);
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

			// Quad
			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
			indices.push_back(i4);
		}
	}
	spline.mesh.SetupMesh(vertices, indices, std::vector<Texture>());
}

void GenerateSurface(SplineModel& spline)
{
	Mesh mesh;
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;

	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	glm::vec3 tempVert(0);
	glm::vec3 tempVertNormalized(0);

	unsigned int controlPointsCount = spline.controlPoints.size();
	unsigned int vectorsCount = spline.controlPointsVectorDir[0].size();

	for (int i = 0; i < controlPointsCount; i++)
	{
		tempVert = spline.controlPointsVectorPos[i][0];
		tempVertNormalized = glm::normalize(spline.controlPointsVectorDir[i][0]);
		for (int j = 0; j < vectorsCount - 1; j++)
		{
			Vertex vertex;
			vertex.Position = spline.controlPointsVectorPos[i][j];
			vertex.TexCoord = glm::vec2(j / vectorsCount, i / (vectorsCount));
			vertex.Normal = glm::normalize(spline.controlPointsVectorDir[i][j]);
			vertices.push_back(vertex);

			if (j == vectorsCount - 1)
			{
				Vertex tempVertex;
				tempVertex.Position = tempVert;
				tempVertex.TexCoord = glm::vec2((j + 1) / vectorsCount, i / (vectorsCount * 0.5f));
				tempVertex.Normal = tempVertNormalized;
				vertices.push_back(tempVertex);
				spline.controlPointsVectorPos[i][j + 1] = tempVert;
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
	Texture tex("resources/textures/4x_tex.png", Texture::Type::DIFFUSE, Texture::Parameter::REPEAT);
	textures.push_back(tex);
	spline.mesh.SetupMesh(vertices, indices, textures);
}
