#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <Shader.h>
#include <camera.h>

#include <VertexArray.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>

#ifdef _DEBUG
	constexpr bool isDebug = true;
#endif
#ifndef _DEBUG
	constexpr bool isDebug = false;
#endif

namespace Debug
{
	struct LineVertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};
	class Line
	{
		private:
			inline static std::vector<LineVertex> m_vertices;

			int m_maxVertices;
			std::unique_ptr<VertexArray> m_VAO;
			std::unique_ptr<VertexBuffer> m_VBO;
			GLenum m_primitiveMode;
			unsigned int m_vertexBufferSize;

			void _create_buffer();
			void _buffer();

			Line(const Line&) = delete;
			Line(int maxVertices = 300);
		
		public:
			static Line& OnStart(int maxVertices = 300);
			void OnUpdate(Shader& shader, const ogl::Camera& camera, float aspectRatio);
			static void Draw(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
			static void Draw(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colorStart, const glm::vec3& colorEnd);

	};
}