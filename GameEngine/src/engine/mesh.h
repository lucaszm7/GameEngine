#pragma once

#include <GLM/glm.hpp>
#include <vector>
#include <format>
#include <memory>

// Core
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

enum class TriangleOrientation
{
	ClockWise = 0,
	CounterClockWise = 1
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

struct Transform
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};

enum class DrawPrimitive
{
	Triangle = GL_FILL,
	Point = GL_POINT,
	WireFrame = GL_LINE
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh() = default;
	Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& indi, const std::vector<Texture>& text);
	void Draw(Shader& shader, DrawPrimitive drawPrimitive = DrawPrimitive::Triangle) const;
	static void Rasterize(std::vector<cgl::vec4>& vertices, DrawPrimitive drawPrimitive = DrawPrimitive::Triangle);
	void SetupMesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& indi, const std::vector<Texture>& text);

private:
	std::shared_ptr<VertexArray>  VAO;
	std::shared_ptr<VertexBuffer> VBO;
	std::shared_ptr<IndexBuffer>  EBO;
	VertexBufferLayout VBL;
	void setupBuffers();
};

