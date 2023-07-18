#include "mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& indi, const std::vector<Texture>& text)
	: vertices(vert), indices(indi), textures(text)
{
	this->setupBuffers();
}

void Mesh::SetupMesh(const std::vector<Vertex>& vert, const std::vector<unsigned int>& indi, const std::vector<Texture>& text)
{
	vertices = vert;
	indices = indi;
	textures = text;
	this->setupBuffers();
}

void Mesh::setupBuffers()
{
	VAO = std::make_shared<VertexArray>();
	VAO->Bind();
	VBO = std::make_shared<VertexBuffer>(&vertices[0], static_cast<unsigned int>(vertices.size() * sizeof(Vertex)), GL_STATIC_DRAW);
	EBO = std::make_shared<IndexBuffer>(&indices[0], static_cast<unsigned int>(indices.size()), GL_STATIC_DRAW);

	VAO->Bind();
	VBO->Bind();
	EBO->Bind();
	VBL.Push<float>(3); // positions
	VBL.Push<float>(3); // normals
	VBL.Push<float>(2); // texCoord
	VAO->AddBuffer(*VBO, VBL);

	VAO->Unbind();
}


void Mesh::Draw(Shader& shader, DrawPrimitive drawPrimitive) const
{
	for (int i = 0; i < textures.size(); ++i)
	{
		textures[i].Bind(i);
		std::string uniformName = std::format("material.{}", Texture::to_string(textures[i].type));
		shader.SetUniform1i(uniformName, i);
	}
	shader.SetUniform1f("material.shininess", 64);
	VAO->Bind();
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)drawPrimitive);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
	VAO->Unbind();

	glActiveTexture(GL_TEXTURE0);
}


