#include "Texture.h"
#include "STB/stb_image.h"

Texture::Texture(const std::string& path, Texture::Type type, Texture::Parameter texParam)
	:m_FilePath(path), type(type)
{
	unsigned char* m_LocalBuffer = nullptr;
	stbi_set_flip_vertically_on_load(true);
	glGenTextures(1, &m_RendererID);
	m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &nrComponents, 0);

	if (m_LocalBuffer)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		GLenum format = 0;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		setTextureParam(texParam);
		stbi_image_free(m_LocalBuffer);
	}
	else
	{
		std::cout << "ERROR\nFAILED TO LOAD TEXTURE\n";
		__cpp_static_assert;
	}
}

Texture::Texture(const char* data, Texture::Parameter texParam)
	:type(Texture::Type::RAW)
{
	if (data)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		setTextureParam(texParam);
	}
	else
	{
		std::cout << "ERROR\nFAILED TO LOAD TEXTURE\n";
		__cpp_static_assert;
	}
}

void Texture::setTextureParam(Texture::Parameter texParam) const
{
	switch (texParam)
	{
		using enum Texture::Parameter;
		case LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		case REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		default:
			std::cout << "ERROR\nMISSING TEXTURE PARAMETER\n";
			break;
	}
}

Texture::~Texture()
{
	// std::cout << "Deleting texture " << m_RendererID << " of\n" << m_FilePath << "\n";
	// glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
