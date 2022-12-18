#pragma once
#pragma once
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <iostream>
#include <string>

enum class TexParam
{
	LINEAR = 0,
	REPEAT = 1
};

class Texture
{
private:
	unsigned int m_RendererID = 0;
	std::string m_FilePath = "";
	unsigned char* m_LocalBuffer = nullptr;
	int m_Width = 0;
	int m_Height = 0;
	int m_BPP = 0;
public:
	Texture(const std::string& path, GLint format = GL_RGB, TexParam texParam = TexParam::LINEAR);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };
private:
	void setTextureParam(TexParam param) const;
};