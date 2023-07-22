#pragma once

#include <string>
#include <vector>
#include <random>

#include <GLM/glm.hpp> 
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "IMGUI/imgui.h"

#include "Shader.h"
#include "mesh.h"
#include "vec4.h"
#include "mat.hpp"
#include "ViewPort.hpp"

struct Model
{
public:
	Model(const std::string& path, TriangleOrientation triOrientation = TriangleOrientation::CounterClockWise)
		:m_Path(path)
	{
		if (m_Path.substr(m_Path.find_last_of('.') + 1) == "in")
			LoadCustomModel(triOrientation);
		else
			LoadClassicModel();
	}

	void Draw(Shader& shader, 
		PRIMITIVE drawPrimitive = PRIMITIVE::Triangle) const;
	
	cgl::mat4 GetModelMatrix() const;
	void OnImGui() const;
	std::vector<Mesh> meshes;
	std::string name;
	std::vector<std::shared_ptr<Texture>> textures_loaded;
	Transform transform;

private:
	std::string m_Path;
	inline static std::unordered_map<std::string, int> m_NamesMap;

	void LoadClassicModel();
	void LoadCustomModel(TriangleOrientation triOrientation);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<std::shared_ptr<Texture>> loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType);
};


