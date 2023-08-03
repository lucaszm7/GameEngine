#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <vector>
#include "vec3.h"
#include "vec2.h"

struct MipMap
{
	unsigned char* m_Buffer;
	unsigned int m_Width, m_Height;
	std::vector<unsigned char*> m_MipMapLevels;
	
	MipMap(unsigned char* buf, unsigned int width, unsigned int height)
		:m_Buffer(buf), m_Width(width), m_Height(height) {}

	void MakeMipMap();
	unsigned char* GetLevel(unsigned int level) { return m_MipMapLevels[level]; };
	static float GetMipMapLevel(float dx, float dy, unsigned int width, unsigned int height);
};

class Texture
{
private:
	unsigned int m_RendererID = 0;
	std::string m_FilePath = "";
	int m_Width = 0;
	int m_Height = 0;
	unsigned char* m_LocalBuffer = nullptr;
	int nrComponents = 0;

	std::shared_ptr<MipMap> m_MipMap;

public:

	enum class Wrap;
	enum class Type;
	enum class Filtering;
	inline static Texture::Filtering globalFilter;

	Texture::Filtering filtering;

	Texture::Type type;

	Texture(const std::string& path,
		Texture::Type type,
		Texture::Wrap texParam = Texture::Wrap::MIRROR,
		Texture::Filtering filtering = Texture::Filtering::TRILLINEAR,
		bool keepLocalBuffer = false);

	Texture(const unsigned char* data, unsigned int width, unsigned int height, 
		Texture::Filtering filtering = Texture::Filtering::NEAREST_NEIGHBOR, 
		Texture::Wrap texParam = Texture::Wrap::MIRROR);

	void Update(const unsigned char* data, unsigned int width, unsigned int height, Texture::Wrap texParam = Texture::Wrap::MIRROR);
	const unsigned char* GetLocalBuffer() const { return m_LocalBuffer; }

	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	static void SetGlobalFiltering(Texture::Filtering filtering, Texture::Wrap texParam = Texture::Wrap::MIRROR);

	void SetFiltering() const;

	int GetWidth() const { return m_Width; };
	int GetHeight() const { return m_Height; };
	int GetID() const { return m_RendererID; };
	std::string GetPath() const { return this->m_FilePath; };

	static cgl::vec3 BilinearFiltering(const unsigned char* const buffer, unsigned int buffer_width, float u, float v);
	static cgl::vec3 GetPixelColorFromTextureBuffer(const unsigned char* const textureBuffer, unsigned int buffer_width, const unsigned int u, const unsigned int v);

	std::shared_ptr<MipMap> GetMipMap() const { return m_MipMap; }

	enum class Wrap
	{
		MIRROR = GL_MIRRORED_REPEAT,
		REPEAT = GL_REPEAT
	};

	enum class Type
	{
		DIFFUSE,
		SPECULAR,
		EMISSION,
		RAW
	};

	enum class Filtering
	{
		NEAREST_NEIGHBOR = GL_NEAREST,
		BILINEAR = GL_LINEAR,
		TRILLINEAR = GL_LINEAR_MIPMAP_LINEAR
	};

	static std::string to_string(Texture::Type type)
	{
		using enum Texture::Type;
		switch (type)
		{
		case DIFFUSE:
			return "diffuse";
		case SPECULAR:
			return "specular";
		case EMISSION:
			return "emission";
		case RAW:
			return "raw";
		}
	}
};