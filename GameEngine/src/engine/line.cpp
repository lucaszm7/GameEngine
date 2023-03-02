#include "engine/line.h"

Line::Line(int maxVertices, std::vector<Eigen::Vector3f> vertices) :
	m_maxVertices(maxVertices), m_vertices(vertices), m_primitiveMode(GL_LINES)
{
	int n = m_vertices.size();
	if (m_maxVertices < n)
		m_maxVertices = n;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	if (n > 0)
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * m_maxVertices, m_vertices[0].data(), GL_DYNAMIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * m_maxVertices, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_vertexBufferSize = m_maxVertices * 3 * sizeof(float);
}

void Line::Buffer()
{
	unsigned int vertexBufferSize = m_vertices.size() * sizeof(Eigen::Vector3f);
	if (vertexBufferSize <= m_vertexBufferSize)
	{
		// Can use same buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBufferSize, m_vertices[0].data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//std::cout << "UPDATE" << std::endl;
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
	//std::cout << sizeof(Vertex) <<std::endl;
}

void Line::CreateBuffer()
{
	unsigned int vertexBufferSize = m_vertices.size() * sizeof(Eigen::Vector3f);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertices[0].data(), GL_DYNAMIC_DRAW);
	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Eigen::Vector3f), (GLvoid*)0);
	// vertex normal
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	//// vertex texture coords
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_vertexBufferSize = vertexBufferSize;
}

void Line::Draw()
{
	glBindVertexArray(m_VAO);
	glDrawArrays(m_primitiveMode, 0, m_vertices.size());
	glBindVertexArray(0);
}