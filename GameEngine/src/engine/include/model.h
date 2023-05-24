#pragma once

#include <string>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "IMGUI/imgui.h"

#include "shader.h"
#include "mesh.h"


struct Model
{
public:
	Model(const std::string& path)
	{
		loadModel(path);
	}
	void Draw(Shader& shader);
	void OnImGui();
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	Transform transform;
private:
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType);
};


