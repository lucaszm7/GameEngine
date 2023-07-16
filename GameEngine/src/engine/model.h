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

struct Pixel
{
	union
	{
		struct
		{
			unsigned char r, g, b;
		};
		unsigned char p[3];
	};
};

inline std::ostream& operator << (std::ostream& out, const Pixel& p)
{
	return out << (unsigned int)p.r << ',' 
			   << (unsigned int)p.g << ',' 
		       << (unsigned int)p.b;
}

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

	void DrawOpenGL(Shader& shader, 
		DrawPrimitive drawPrimitive = DrawPrimitive::Triangle) const;
	
	cgl::mat4 GetModelMatrix() const;
	void OnImGui() const;
	std::vector<Mesh> meshes;
	std::string name;
	std::vector<Texture> textures_loaded;
	Transform transform;

	void DrawCGL(Shader& shader,
		DrawPrimitive drawPrimitive, 
		const cgl::mat4& view, 
		const cgl::mat4& projection, 
		const cgl::mat4& viewport,
		bool isCulling = false, 
		bool isCullingClockWise = false) const;

	static void ViewPort(const unsigned int screenWidth, const unsigned int screenHeight);
	static void ClearFrameBuffer();
	static void ClearZBuffer();
	static cgl::mat<glm::vec3>* GetFrameBuffer() { return &m_FrameBuffer; };

private:
	std::string m_Path;
	inline static std::unordered_map<std::string, int> m_NamesMap;

	inline static cgl::mat<glm::vec3> m_FrameBuffer;
	inline static cgl::mat<unsigned int> m_ZBuffer;

	void LoadClassicModel();
	void LoadCustomModel(TriangleOrientation triOrientation);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType);
};


