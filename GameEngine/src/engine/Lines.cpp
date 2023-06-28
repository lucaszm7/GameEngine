#include "Lines.hpp"

namespace Debug
{
	Line::Line(int maxVertices,
		const std::vector<glm::vec3>& vertices,
		const glm::vec3 color)
	{
		m_maxVertices = maxVertices;
		m_vertices = vertices;
		m_uColor = color;
		m_primitiveMode = GL_LINES;

		int n = m_vertices.size();
		if (m_maxVertices < n)
			m_maxVertices = n;

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		if (n > 0)
			glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * m_maxVertices, &m_vertices[0], GL_DYNAMIC_DRAW);
		else
			glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * m_maxVertices, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_vertexBufferSize = m_maxVertices * 3 * sizeof(float);
	}

	Line& Line::OnStart(int maxVertices,
		const std::vector<glm::vec3>& vertices,
		const glm::vec3 color)
	{
		static Line instance(maxVertices, vertices, color);
		return instance;
	}

	void Line::OnUpdate(Shader& shader, const ogl::Camera& camera, float aspectRatio)
	{
		shader.Bind();
		shader.SetUniformMatrix4fv("model", glm::mat4(1.0f));
		shader.SetUniformMatrix4fv("view", camera.GetViewMatrix());
		shader.SetUniformMatrix4fv("projection", camera.GetProjectionMatrix(aspectRatio));

		shader.SetUniform3f("lightColor", m_uColor);
		_buffer();

		glBindVertexArray(m_VAO);
		glDrawArrays(m_primitiveMode, 0, m_vertices.size());
		glBindVertexArray(0);
		shader.Unbind();
		m_vertices.clear();
	}

	void Line::Draw(const glm::vec3& start, const glm::vec3& end)
	{
		m_vertices.push_back(start);
		m_vertices.push_back(end);
	}

	void Line::_buffer()
	{
		unsigned int vertexBufferSize = m_vertices.size() * sizeof(glm::vec3);
		if (vertexBufferSize <= m_vertexBufferSize && vertexBufferSize > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBufferSize, &m_vertices[0]);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (m_vertexBufferSize == 0)
		{
			_create_buffer();
		}
		else
		{
			glDeleteVertexArrays(1, &m_VAO);
			glDeleteBuffers(1, &m_VBO);
			_create_buffer();
		}
	}

	void Line::_create_buffer()
	{
		unsigned int vertexBufferSize = m_vertices.size() * sizeof(glm::vec3);

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		if (vertexBufferSize > 0)
			glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, &m_vertices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_vertexBufferSize = vertexBufferSize;
	}
}