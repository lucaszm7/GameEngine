#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <Shader.h>
#include <camera.h>

namespace Debug
{
	class Line
	{
		private:
			inline static std::vector<glm::vec3> m_vertices;
			inline static glm::vec3 m_uColor;
			
			int m_maxVertices;
			GLuint m_VAO;
			GLuint m_VBO;
			GLenum m_primitiveMode;
			unsigned int m_vertexBufferSize;

			void _create_buffer();
			void _buffer();

			Line(const Line&) = delete;
			Line(int maxVertices = 300,
				const std::vector<glm::vec3>& vertices = {},
				const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f));
		
		public:
			static Line& OnStart(int maxVertices = 300,
				const std::vector<glm::vec3>& vertices = {},
				const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f));
			void OnUpdate(Shader& shader, const ogl::Camera& camera, float aspectRatio);
			static void Draw(const glm::vec3& start, const glm::vec3& end);
	};
}