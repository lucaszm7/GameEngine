#include "Lines.hpp"

namespace Debug
{
	Line::Line(int maxVertices)
		:m_maxVertices(maxVertices)
		,m_primitiveMode(GL_LINES)
	{
		m_vertexBufferSize = m_maxVertices * sizeof(LineVertex);
		m_vertices.reserve(m_vertexBufferSize);
		_create_buffer();
	}

	Line& Line::OnStart(int maxVertices)
	{
		static Line instance(maxVertices);
		return instance;
	}

	void Line::OnUpdate(Shader& shader, const ogl::Camera& camera, float aspectRatio)
	{
		if (m_vertices.empty())
			return;

		shader.Bind();
		shader.SetUniformMatrix4fv("view", camera.GetViewMatrix());
		shader.SetUniformMatrix4fv("projection", camera.GetProjectionMatrix(aspectRatio));

		_buffer();

		m_VAO->Bind();
		glDrawArrays(m_primitiveMode, 0, m_vertices.size());
		m_VAO->Unbind();
		shader.Unbind();
		m_vertices.clear();
	}

	void Line::Draw(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
	{
		m_vertices.emplace_back(start, color);
		m_vertices.emplace_back(end, color);
	}

	void Line::Draw(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colorStart, const glm::vec3& colorEnd)
	{
		m_vertices.emplace_back(start, colorStart);
		m_vertices.emplace_back(end, colorEnd);
	}

	void Line::_buffer()
	{
		unsigned int vertexBufferSize = m_vertices.size() * sizeof(LineVertex);
		if (vertexBufferSize <= m_vertexBufferSize)
			m_VBO->Update(m_vertices.data(), vertexBufferSize);
		else
			_create_buffer();
	}

	void Line::_create_buffer()
	{
		if (m_VAO)
			m_VAO.release();
		if (m_VBO)
			m_VBO.release();

		m_vertexBufferSize = m_vertices.size() * sizeof(LineVertex);
		m_VAO = std::make_unique<VertexArray>();
		m_VBO = std::make_unique<VertexBuffer>(m_vertices.data(), m_vertexBufferSize, GL_DYNAMIC_DRAW);

		VertexBufferLayout VBL;
		VBL.Push<float>(3);
		VBL.Push<float>(3);

		m_VAO->AddBuffer(*m_VBO, VBL);
	}
}