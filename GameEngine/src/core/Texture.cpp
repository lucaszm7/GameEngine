#include "Texture.h"
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <format>

Texture::Texture(const std::string& path, 
	Texture::Type type, 
	Texture::Wrap texParam, 
	Texture::Filtering filtering,
	bool keepLocalBuffer)
	:m_FilePath(path), type(type), filtering(filtering)
{
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

		SetGlobalFiltering(filtering, texParam);

		if (!keepLocalBuffer)
			stbi_image_free(m_LocalBuffer);
		else
		{
			m_MipMap = std::make_shared<MipMap>(m_LocalBuffer, m_Width, m_Height);
			m_MipMap->MakeMipMap();
	}
	}
	else
	{
		std::cout << "ERROR\nFAILED TO LOAD TEXTURE\n";
		__cpp_static_assert;
	}
}

Texture::Texture(const unsigned char* data, unsigned int width, unsigned int height, Texture::Filtering filtering, Texture::Wrap texParam)
	:type(Texture::Type::RAW), m_Width(width), m_Height(height), filtering(filtering)
{
	if (data)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		SetGlobalFiltering(filtering, texParam);
	}
	else
	{
		std::cout << "ERROR\nFAILED TO LOAD TEXTURE\n";
		__cpp_static_assert;
	}
}

void Texture::Update(const unsigned char* data, unsigned int width, unsigned int height, Texture::Wrap texParam)
{
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		if (width > m_Width || height > m_Height)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR\nFAILED TO LOAD TEXTURE: NO DATA PROVIDED\n";
		__cpp_static_assert;
	}
}

Texture::~Texture()
{
#ifdef _DEBUG
	if(m_FilePath != "")
		std::cout << "Deleting texture [" << m_RendererID << "] of " << m_FilePath << "\n";
#endif
	glDeleteTextures(1, &m_RendererID);
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

void Texture::SetGlobalFiltering(Texture::Filtering filtering, Texture::Wrap texParam)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)texParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)texParam);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)filtering);
}

void Texture::SetFiltering() const
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)Texture::Wrap::REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)Texture::Wrap::REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)filtering);
}

void MipMap::MakeMipMap()
{
	unsigned int layer_width = m_Width;
	unsigned int layer_height = m_Height;

	m_MipMapLevels.push_back(m_Buffer);

	while ((layer_width > 2 && layer_height > 2) && m_MipMapLevels.size() < 7)
	{
#ifdef _DEBUG
		stbi_write_jpg(std::format("mipmap_level_{}.jpg", m_MipMapLevels.size() - 1).c_str(), layer_width, layer_height, 3, m_MipMapLevels.back(), 100);
#endif

		layer_width  = std::floor((float)layer_width  / 2.0f);
		layer_height = std::floor((float)layer_height / 2.0f);

		auto current_buffer = m_MipMapLevels.back();
		unsigned char* new_layer_buffer = new unsigned char[layer_width * layer_height * 3];

		for (unsigned int j = 0; j < (layer_height * 2); j += 2)
		{
			for (unsigned int i = 0; i < (layer_width * 2); i += 2)
			{
				cgl::vec3 pixelColor = Texture::BilinearFiltering(current_buffer, layer_width * 2, (float)i + 0.5f, (float)j + 0.5f);
				new_layer_buffer[((j/2) * layer_height + (i/2)) * 3 + 0] = (unsigned char)(pixelColor.x * 255.0f);
				new_layer_buffer[((j/2) * layer_height + (i/2)) * 3 + 1] = (unsigned char)(pixelColor.y * 255.0f);
				new_layer_buffer[((j/2) * layer_height + (i/2)) * 3 + 2] = (unsigned char)(pixelColor.z * 255.0f);
			}
		}
		m_MipMapLevels.push_back(new_layer_buffer);
	}
}

float MipMap::GetMipMapLevel(float dx, float dy, unsigned int width, unsigned int height)
{
	return 0.0f;
}
