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

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& indi, const std::vector<Texture>& text);
	void Draw(Shader& shader);

private:
	std::shared_ptr<VertexArray>  VAO;
	std::shared_ptr<VertexBuffer> VBO;
	std::shared_ptr<IndexBuffer>  EBO;
	VertexBufferLayout VBL;

	void setupMesh();
};

Mesh::Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& indi, const std::vector<Texture>& text)
	: vertices(vert), indices(indi), textures(text) 
{
	VAO = std::make_shared<VertexArray>();
	VAO->Bind();
	VBO = std::make_shared<VertexBuffer>(&vertices[0], static_cast<unsigned int>(vertices.size() * sizeof(Vertex)), GL_STATIC_DRAW);
	EBO = std::make_shared<IndexBuffer> (&indices[0],  static_cast<unsigned int>(indices.size()), GL_STATIC_DRAW);
	this->setupMesh();
}

void Mesh::setupMesh()
{
	VAO->Bind();
	VBO->Bind();
	EBO->Bind();
	VBL.Push<float>(3); // positions
	VBL.Push<float>(3); // normals
	VBL.Push<float>(2); // texCoord
	VAO->AddBuffer(*VBO, VBL);

	VAO->Unbind();
}

void Mesh::Draw(Shader& shader)
{
	for (int i = 0; i < textures.size(); ++i)
	{
		textures[i].Bind(i);
		std::string uniformName = std::format("material.{}", Texture::to_string(textures[i].type));
		shader.SetUniform1i(uniformName, i);
	}
	shader.SetUniform1f("material.shininess", 64);
	VAO->Bind();
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
	VAO->Unbind();

	glActiveTexture(GL_TEXTURE0);
}
