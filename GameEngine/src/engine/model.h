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
	Model(const std::string& path, glm::vec3 col = glm::vec3(1.0f,0.0f,0.0f), TriangleOrientation triOrientation = TriangleOrientation::CounterClockWise)
		:color(col), m_Path(path)
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
	glm::vec3 color;
	std::vector<Texture> textures_loaded;
	Transform transform;

	void DrawSoftwareRasterized(
		DrawPrimitive drawPrimitive,
		const cgl::mat4& view,
		const cgl::mat4& projection,
		const cgl::mat4& viewport, 
		unsigned int screenWidth, 
		unsigned int screenHeight, 
		bool isCulling = false, 
		bool isCullingClockWise = false) const;

	static void SetViewPort(const unsigned int screenWidth, const unsigned int screenHeight);
	static void ClearFrameBuffer();
	static void SetClearColor(const Pixel& p) { m_ClearColor = p; };
	static void ClearZBuffer();
	static cgl::mat<Pixel>* GetFrameBuffer() { return &m_FrameBuffer; };

private:
	std::string m_Path;
	inline static std::unordered_map<std::string, int> m_NamesMap;

	ViewPort m_MapToViewport;
	inline static Pixel m_ClearColor = Pixel{ 255,255,255 };
	inline static cgl::mat<Pixel> m_FrameBuffer;
	inline static cgl::mat<float> m_ZBuffer;

	void LoadClassicModel();
	void LoadCustomModel(TriangleOrientation triOrientation);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, Texture::Type textureType);
};


