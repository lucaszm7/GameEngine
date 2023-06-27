#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include "material.h"
#include "light.h"

#include <vec3.h>
#include <mat4.h>

class Shader
{
private:
	std::string m_VertexShaderPath;
	std::string m_FragmentShaderPath;

	// Program ID
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();
	// We call bind here just for consistency, but in behind its actully:
	// gl_useProgram();
	void Bind() const;
	void Unbind() const;
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, const glm::vec3& v);
	void SetUniform3f(const std::string& name, const cgl::vec3& v);
	void SetUniform1i(const std::string& name, int v0);
	void SetUniform1f(const std::string& name, float v0);
	void SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat4);
	void SetUniformMatrix4fv(const std::string& name, const cgl::mat4& mat4);
	
	void SetUniformMaterial(const Material& mat);
	void SetUniformLight(const DirectionalLight& light);
	void SetUniformLight(const PointLight& light);
	void SetUniformLight(const std::vector<PointLight>& lights);
	void SetUniformLight(const SpotLight& light);

private:
	std::string ParseShader(const std::string& filepath) const;
	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) const;
	int GetUniformLocation(const std::string& name);
};