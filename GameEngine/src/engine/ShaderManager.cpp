#include "ShaderManager.h"

void ShaderManager::Init()
{
	static bool isInit = false;
	if (isInit) 
		return;
	else
		isInit = true;

#ifdef _DEBUG
	std::cout << "Loading shaders..." << std::endl;
	std::cout << "\tDEFAULT\n";
	std::cout << "\tSHADOW_SPOT\n";
	std::cout << "\tSOLITUDE\n";
#endif
	AddShader(SHADER_TYPE::DEFAULT,		"resources/shaders/default_vertex.shader",
										"resources/shaders/default_fragment.shader");
	AddShader(SHADER_TYPE::SHADOW_SPOT, "../GameEngine/resources/shader/ShadowMap/shadowMap_vertex.shader", 
										"../GameEngine/resources/shader/ShadowMap/shadowMap_fragment.shader");
	AddShader(SHADER_TYPE::SOLITUDE,	"resources/shaders/Solitude/vertex.shader",
										"resources/shaders/Solitude/fragment.shader");
	AddShader(SHADER_TYPE::OPENGL, "resources/shaders/ogl_vertex.shader", "resources/shaders/ogl_fragment.shader");
}

void ShaderManager::AddShader(const SHADER_TYPE& st, const std::string& vs, const std::string& fs)
{
	shaders.emplace(std::piecewise_construct, std::make_tuple(st), std::make_tuple(vs, fs));
}

const Shader& ShaderManager::GetShader(const SHADER_TYPE& st)
{
	return shaders.at(st);
}
