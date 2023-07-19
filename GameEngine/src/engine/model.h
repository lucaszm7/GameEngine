#pragma once

#include <string>
#include <vector>

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
	Model(const std::string& path, glm::vec3 col = glm::vec3(1.0f,0.0f,0.0f), TriangleOrientation triOrientation = TriangleOrientation::CounterClockWise)
		:color(col), m_Path(path)
	{
		if (m_Path.substr(m_Path.find_last_of('.') + 1) == "in")
			LoadCustomModel(triOrientation);
		else
			LoadClassicModel();
	}

	void Draw(Shader& shader, 
		DrawPrimitive drawPrimitive = DrawPrimitive::Triangle) const;
	
	cgl::mat4 GetModelMatrix() const;
	void OnImGui() const;
	std::vector<Mesh> meshes;
	std::string name;
	glm::vec3 color;
	std::vector<Texture> textures_loaded;
	Transform transform;

private:
	std::string m_Path;
	inline static std::unordered_map<std::string, int> m_NamesMap;

	void LoadClassicModel();
	void LoadCustomModel(TriangleOrientation triOrientation);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType);
};


