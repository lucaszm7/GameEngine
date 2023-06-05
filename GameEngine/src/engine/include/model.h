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

enum ModelType
{
	CLASSIC,
	CUSTOM
};

struct Model
{
public:
	Model(const std::string& path, ModelType modelType = ModelType::CLASSIC)
	{
		if (modelType == ModelType::CLASSIC)
			LoadClassicModel(path);
		else if (modelType == ModelType::CUSTOM)
			LoadCustomModel(path);
		else
			throw new std::exception("Model Type not Supported.");
	}
	void Draw(Shader& shader);
	void OnImGui();
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	Transform transform;
private:
	void LoadClassicModel(const std::string& path);
	void LoadCustomModel(const std::string& path);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType);
};


