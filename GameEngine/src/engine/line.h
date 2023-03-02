#include <GL/glew.h>    
#include <vector>
#include <Eigen/Core>
#include <Eigen/Dense>

class Line
{
public:
	std::vector<Eigen::Vector3f> m_vertices;
	int m_maxVertices;
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_mvpID;
	GLenum m_primitiveMode;
	unsigned int m_vertexBufferSize;
	Line(int maxVertices = 30, std::vector<Eigen::Vector3f> vertices = {});
	void Buffer();
	void CreateBuffer();
	void Draw();
};