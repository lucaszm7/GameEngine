#include <vector>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <Shader.h>

namespace cgl
{
	class Line
	{
		private:
			std::vector<glm::vec3> m_vertices;
			int m_maxVertices;
			GLuint m_VAO;
			GLuint m_VBO;
			GLenum m_primitiveMode;
			unsigned int m_vertexBufferSize;
			void CreateBuffer()
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
			void Buffer()
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
					CreateBuffer();
				}
				else
				{
					glDeleteVertexArrays(1, &m_VAO);
					glDeleteBuffers(1, &m_VBO);
					CreateBuffer();
				}
			}

		public:
			Line(int maxVertices = 300, const std::vector<glm::vec3>& vertices = {})
			{
				m_maxVertices = maxVertices;
				m_vertices = vertices; 
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

			void Draw(Shader& shader)
			{
				shader.SetUniformMatrix4fv("model", glm::mat4(1.0));
				shader.SetUniform3f("uColor", glm::vec3(0.0f, 1.0f, 0.0f));
				Buffer();

				glBindVertexArray(m_VAO);
				glDrawArrays(m_primitiveMode, 0, m_vertices.size());
				glBindVertexArray(0);

				Reset();
			}

			void Add(const glm::vec3& start, const glm::vec3& end)
			{
				m_vertices.push_back(start);
				m_vertices.push_back(end);
			}

			void Reset()
			{
				m_vertices.clear();
			}
	};
}