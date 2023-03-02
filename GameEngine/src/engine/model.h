#pragma once

#include <string>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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

void Model::Draw(Shader& shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, transform.scale);
	shader.SetUniformMatrix4fv("model", model);

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(shader);
	}
}

void Model::OnImGui()
{
	if (ImGui::TreeNode("Transform"))
	{
		ImGui::DragFloat3("Colon Position:", &transform.position[0], 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3("Colon Rotation:", &transform.rotation[0], 0.1f, -glm::pi<float>(), glm::pi<float>());
		ImGui::DragFloat3("Colon Scale:", &transform.scale[0], 0.01f, -10.0f, 10.0f);
		ImGui::TreePop();
	}
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

		if(mesh->HasNormals())
			vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

		if (mesh->mTextureCoords[0]) // Has texture coords
			vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

		else
			vertex.TexCoord = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Textures
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
	std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
	std::vector<Texture> emissionMaps = loadMaterialTexture(material, aiTextureType_EMISSION_COLOR, Texture::Type::EMISSION);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); ++j)
		{
			std::string texPath = textures_loaded[j].GetPath();
			std::string fileName = texPath.substr(texPath.find_last_of('/') + 1, texPath.size());
			if (std::strcmp(fileName.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture(directory + "/" + std::string(str.C_Str()), textureType, Texture::Parameter::REPEAT);
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

