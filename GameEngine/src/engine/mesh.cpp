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

void Mesh::Rasterize(std::vector<cgl::vec4>& vertices, DrawPrimitive drawPrimitive)
{
	for (unsigned int i = 0; i < vertices.size(); i += 3)
	{
		auto& p0 = vertices[i+0];
		auto& p1 = vertices[i+1];
		auto& p2 = vertices[i+2];

		// Ordena de forma decrescente em Y (top to bottom)
		if (p1.y < p0.y)
			p0 = p1;
		if (p2.y < p0.y)
			p0 = p2;
		if (p2.y < p1.y)
			p1 = p2;

		cgl::vec3 edge0 = (p1 - p0).to_vec3();
		cgl::vec3 edge1 = (p2 - p0).to_vec3();

		int incrementX = (edge0.x / edge0.y);
		int incrementZ = (edge0.z / edge0.y);


	}
}

