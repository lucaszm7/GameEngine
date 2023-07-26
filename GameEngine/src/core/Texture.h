#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <iostream>
#include <string>


class Texture
{
private:
	unsigned int m_RendererID = 0;
	std::string m_FilePath = "";
	int m_Width = 0;
	int m_Height = 0;
	unsigned char* m_LocalBuffer = nullptr;
	int nrComponents = 0;
public:
	enum class Parameter;
	enum class Type;
	Texture::Type type;

	Texture(const std::string& path, Texture::Type type, Texture::Parameter texParam = Texture::Parameter::LINEAR, bool keepLocalBuffer = false);
	Texture(const unsigned char* data, unsigned int width, unsigned int height, Texture::Parameter texParam = Texture::Parameter::LINEAR);

	void Update(const unsigned char* data, unsigned int width, unsigned int height, Texture::Parameter texParam = Texture::Parameter::LINEAR);
	unsigned char* GetLocalBuffer() const { return m_LocalBuffer; }

	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };
	inline int GetID() const { return m_RendererID; };
	inline std::string GetPath() const { return this->m_FilePath; };

private:
	void setTextureParam(Texture::Parameter param) const;

public:
	enum class Parameter
	{
		LINEAR,
		REPEAT
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
		TRILLINEAR
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