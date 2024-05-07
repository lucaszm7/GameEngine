#pragma once

#include <vector>
#include <unordered_map>

#include "Shader.h"

enum class SHADER_TYPE
{
	DEFAULT,
	SHADOW_DIRECT,
	SHADOW_POINT,
	SHADOW_SPOT,
	SHADOW_DEBUG,
	SOLITUDE,
	OPENGL
};

class ShaderManager
{
public:
	ShaderManager() = default;
	~ShaderManager() = default;

	static void Init();
	static void AddShader(const SHADER_TYPE& st, const std::string& vs, const std::string& fs);
	static const Shader& GetShader(const SHADER_TYPE& st);

private:
	inline static std::unordered_map<SHADER_TYPE, Shader> shaders;
};

