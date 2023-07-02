#pragma once

#include <string>
#include <vector>

#include <GLM/glm.hpp> 
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "IMGUI/imgui.h"

#include "shader.h"
#include "mesh.h"
#include "vec4.h"

struct Model
{
public:
	Model(const std::string& path, TriangleOrientation triOrientation = TriangleOrientation::CounterClockWise)
	{
		if (path.substr(path.find_last_of('.') + 1) == "in")
			LoadCustomModel(path, triOrientation);
		else
			LoadClassicModel(path);
	}

	cgl::mat4 GetModelMatrix() const;
	void DrawOpenGL(Shader& shader, DrawPrimitive drawPrimitive = DrawPrimitive::Triangle) const;
	void DrawCGL(Shader& shader, DrawPrimitive drawPrimitive, const cgl::mat4& view, const cgl::mat4& projection) const;
	void OnImGui() const;
	std::vector<Mesh> meshes;
	std::string name;
	std::vector<Texture> textures_loaded;
	Transform transform;
private:
	inline static std::unordered_map<std::string, int> m_NamesMap;

	void LoadClassicModel(const std::string& path);
	void LoadCustomModel(const std::string& path, TriangleOrientation triOrientation);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType);
};


