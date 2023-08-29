#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

class Cube
{
private:
	std::unique_ptr<Shader> shader;
	VertexArray VAO;
	std::unique_ptr<VertexBuffer> VBO;

public:
	Cube()
	{
		static const std::vector<float> cubeVertices =
		{
			// positions
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

		shader = std::make_unique<Shader>("resources/shaders/light_vertex.shader", "resources/shaders/light_fragment.shader");
		VAO.Bind();
		VBO = std::make_unique<VertexBuffer>(&cubeVertices[0], static_cast<unsigned int>(cubeVertices.size() * sizeof(float)), GL_STATIC_DRAW);
		VertexBufferLayout lightVBL;
		lightVBL.Push<float>(3); // positions
		VAO.AddBuffer(*VBO, lightVBL);
	}

	Cube(const Cube&) = default;

	void Draw(const glm::vec3 position, glm::vec3 color, const ogl::Camera& camera, float aspectRatio)
	{
		glDisable(GL_CULL_FACE);

		auto view = camera.GetViewMatrix();
		auto projection = camera.GetProjectionMatrix(aspectRatio);

		shader->Bind();
		shader->SetUniformMatrix4fv("view", view);
		shader->SetUniformMatrix4fv("projection", projection);

		auto model = glm::translate(glm::mat4(1.0f), position);
		model	   = glm::scale(model, glm::vec3(0.3f));

		shader->SetUniformMatrix4fv("model", model);

		shader->SetUniform3f("lightColor", color);

		VAO.Bind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		VAO.Unbind();

		glEnable(GL_CULL_FACE);
	}
};
