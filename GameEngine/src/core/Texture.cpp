#include "Texture.h"
#include "stb_image.h"
#include <format>
#include <tuple>
#include <fstream>

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

Texture::Texture(unsigned int width, unsigned int height, Texture::Type type)
	:type(type), m_Width(width), m_Height(height)
{
	if (type == Type::MULTISAMPLED)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, m_Width, m_Height, GL_TRUE);

		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}

	else
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Unbind();
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

cgl::vec3 Texture::GetPixelColorFromTextureBuffer(const unsigned char* const textureBuffer, unsigned int buffer_width, const unsigned int u, const unsigned int v)
{
	auto currentPixelColor = &textureBuffer[(v * buffer_width + u) * 3];
	return { ((float)currentPixelColor[0]) / 255.0f, ((float)currentPixelColor[1]) / 255.0f, ((float)currentPixelColor[2]) / 255.0f };
}

void Texture::FromPixelArrayToASCII(const std::string& image_path)
{
	const auto imageData  = GetPixelArrayFromImage(image_path);
	const auto pixelArray = std::get<0>(imageData);
	const auto width      = std::get<1>(imageData);
	const auto height     = std::get<2>(imageData);

	std::cout << "Width: " << width << " Height: " << height << "\n";

	std::string asciiArt;
	std::string greyscale_0 = ".:+#$";
	std::string greyscale_1 = "@#*+-:.";
	std::string greyscale_2 = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

	asciiArt.reserve(width * height);
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			auto& currentPixelColor = pixelArray[(j * width) + i];
			auto r = (int)(currentPixelColor[0]);
			auto g = (int)(currentPixelColor[1]);
			auto b = (int)(currentPixelColor[2]);
			auto gray = (r + g + b) / 3;

			gray = (255 - gray);
			auto char_index = (gray * (greyscale_1.size() - 1)) / 255;
			asciiArt += greyscale_1[char_index];
		}
		asciiArt += '\n';
	}

	std::ofstream file("ascii_art.txt");
	file << asciiArt;
	file.close();
}

std::tuple<std::vector<cgl::vec3>, int, int> Texture::GetPixelArrayFromImage(const std::string& image_path)
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(image_path.c_str(), &width, &height, &nrChannels, 3);
	stbi_set_flip_vertically_on_load(true);

	std::vector<cgl::vec3> pixelArray;
	pixelArray.reserve(width * height);

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			auto currentPixelColor = &data[((j * width) + i) * 3];
			pixelArray.push_back({ ((float)currentPixelColor[0]), ((float)currentPixelColor[1]), ((float)currentPixelColor[2]) });
		}
	}

	stbi_image_free(data);
	return { pixelArray, width, height };
}

cgl::vec3 Texture::BilinearFiltering(const unsigned char* const buffer, unsigned int buffer_width, float u, float v)
{
	cgl::vec2 texelPos(u, v);
	cgl::vec2 cellPos(std::floor(u), std::floor(v));

	auto t = texelPos - cellPos;

	// Samples
	cgl::vec3 pixelTL = GetPixelColorFromTextureBuffer(buffer, buffer_width, cellPos.x + 0, cellPos.y + 0);
	cgl::vec3 pixelTR = GetPixelColorFromTextureBuffer(buffer, buffer_width, cellPos.x + 1, cellPos.y + 0);
	cgl::vec3 pixelBL = GetPixelColorFromTextureBuffer(buffer, buffer_width, cellPos.x + 0, cellPos.y + 1);
	cgl::vec3 pixelBR = GetPixelColorFromTextureBuffer(buffer, buffer_width, cellPos.x + 1, cellPos.y + 1);

	cgl::vec3 pixelTX = pixelTR * t.x + pixelTL * (1.0f - t.x);
	cgl::vec3 pixelBX = pixelBR * t.x + pixelBL * (1.0f - t.x);

	return pixelBX * t.y + pixelTX * (1.0f - t.y);
}

cgl::vec3 Texture::BicubicFiltering(const unsigned char* const buffer, unsigned int buffer_width, unsigned int buffer_height, float u, float v)
{
	cgl::vec2 texelPos(u, v);
	cgl::vec2 cellPos(std::floor(u), std::floor(v));

	auto t = texelPos - cellPos;

	std::array<std::array<cgl::vec3, 4>, 4> pixelSplines;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			pixelSplines[y][x] = GetPixelColorFromTextureBuffer(buffer, buffer_width, std::clamp(cellPos.x + (x - 1), 0.0f, (float)buffer_width), std::clamp(cellPos.y + (y - 1), 0.0f, (float)buffer_height));
		}
	}

	auto tt = t * t;
	auto ttt = tt * t;

	// Catmull-rom spline
	auto q1 = 0.5f * (-ttt + 2.0f * tt - t);
	auto q2 = 0.5f * (3.0f * ttt - 5.0f * tt + cgl::vec2{2.0f, 2.0f});
	auto q3 = 0.5f * (-3.0f * ttt + 4.0f * tt + t);
	auto q4 = 0.5f * (ttt - tt);

	// First interpolation
	std::array<cgl::vec3, 4> columnSplinePixels;
	for (int i = 0; i < 4; ++i)
	{
		columnSplinePixels[i] = pixelSplines[i][0] * q1.x + pixelSplines[i][1] * q2.x + pixelSplines[i][2] * q3.x + pixelSplines[i][3] * q4.x;
	}

	auto finalPixelColor = columnSplinePixels[0] * q1.y + columnSplinePixels[1] * q2.y + columnSplinePixels[2] * q3.y + columnSplinePixels[3] * q4.y;

	return { std::clamp(finalPixelColor.x, 0.0f, 1.0f), std::clamp(finalPixelColor.y, 0.0f, 1.0f), std::clamp(finalPixelColor.z, 0.0f, 1.0f) };
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

float MipMap::GetMipMapLevel(float ds, float dt)
{
	auto dist = std::sqrt((ds * ds) + (dt * dt));
	auto level = std::log2(std::max(dist, 1.0f));
	return level;
}
