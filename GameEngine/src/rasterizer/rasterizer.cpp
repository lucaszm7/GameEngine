#include "rasterizer.hpp"

static Pixel to_pixel(const glm::vec3& c)
{
	return { (unsigned char)(c.x * 255), (unsigned char)(c.y * 255), (unsigned char)(c.z * 255) };
}

static Pixel to_pixel(const cgl::vec3& c)
{
	return { (unsigned char)(c.x * 255.0f), (unsigned char)(c.y * 255.0f), (unsigned char)(c.z * 255.0f) };
}

void Rasterizer::SetViewPort(const unsigned int screenWidth, const unsigned int screenHeight)
{
	m_FrameBuffer.resize(screenHeight, screenWidth);
	m_ZBuffer.resize(screenHeight, screenWidth);

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
}

void Rasterizer::ClearFrameBuffer()
{
	m_FrameBuffer.clear(m_ClearColor);
}

void Rasterizer::ClearZBuffer()
{
	m_ZBuffer.clear(std::numeric_limits<float>::max());
}

void Rasterizer::DrawSoftwareRasterized(
	const Model& model,
	const cgl::Camera& camera,
	DirectionalLight& DirectionalLight,
	PRIMITIVE primitive,
	SHADING shading,
	bool showTextures,
	bool isCulling,
	bool isCullingClockWise,
	Texture::Filtering textureFiltering)
{
	m_Shading = shading;
	m_Primitive = primitive;
	m_DirectionalLight = DirectionalLight;
	m_ShowTexture = showTextures;
	m_Filtering = textureFiltering;

	// Build Model Matrix
	cgl::mat4 translate = cgl::mat4::translate(cgl::vec4(model.transform.position, 1.0f));
	cgl::mat4 rotation =  Quaternion::rotation_matrix({1.0f, 0.0f, 0.0f}, model.transform.rotation.x);
	rotation = rotation * Quaternion::rotation_matrix({0.0f, 1.0f, 0.0f}, model.transform.rotation.y);
	rotation = rotation * Quaternion::rotation_matrix({0.0f, 0.0f, 1.0f}, model.transform.rotation.z);
	cgl::mat4 scale = cgl::mat4::scale(model.transform.scale);
	cgl::mat4 modelM = translate * rotation * scale;

	// Build View Matrix
	cgl::mat4 view = camera.GetViewMatrix();

	// Build Projection Matrix
	cgl::mat4 projection = camera.GetProjectionMatrix((float)m_screenWidth / (float)m_screenHeight);

	// Buil ViewPort Matrix
	cgl::mat4 viewport = cgl::mat4::viewport(m_screenWidth, m_screenHeight);

	// ================
	// Build MVP Matrix
	// ================
	cgl::mat4 mvp = projection * view * modelM;

	cgl::mat4 modelView_transposed_inversed = (modelM).inverse().transpose();

	for (unsigned int i = 0; i < model.meshes.size(); ++i)
	{
		std::vector<cgl::vec4> cglVertices;
		std::vector<cgl::vec4> cglColors;
		std::vector<cgl::vec4> cglNormals;
		std::vector<cgl::vec3> cglUVs;

		cglVertices.reserve(model.meshes[i].vertices.size());
		cglColors.reserve(model.meshes[i].vertices.size());
		cglNormals.reserve(model.meshes[i].vertices.size());
		cglUVs.reserve(model.meshes[i].vertices.size());


		if (m_ShowTexture)
			m_CurrentTexture = !model.meshes[i].textures.empty() ? model.meshes[i].textures[0] : nullptr;

		for (unsigned int j = 0; j < model.meshes[i].vertices.size(); j += 3)
		{
			// ===============================
			// Go To Homogeneus Clipping Space
			// ===============================

			// Vertex Transforms
			cgl::vec4 v0 = mvp * cgl::vec4(model.meshes[i].vertices[j + 0].Position, 1.0f);
			cgl::vec4 v1 = mvp * cgl::vec4(model.meshes[i].vertices[j + 1].Position, 1.0f);
			cgl::vec4 v2 = mvp * cgl::vec4(model.meshes[i].vertices[j + 2].Position, 1.0f);

			// Clipping
			if (!v0.is_in_range(v0.w) || !v1.is_in_range(v1.w) || !v2.is_in_range(v2.w))
				continue;

			// Culling
			if (isCulling)
			{
				cgl::vec3 u = (v1 - v0).to_vec3();
				cgl::vec3 v = (v2 - v0).to_vec3();
				float sign = (u.x * v.y) - (v.x * u.y);
				if (isCullingClockWise && sign > 0.0f)
					continue;
				if (!isCullingClockWise && sign < 0.0f)
					continue;
			}

			// ===================================
			// Go To Normalized Device Coordinates
			// ===================================

			// Save w for perspective correct interpolation
			float v0w = v0.w;
			float v1w = v1.w;
			float v2w = v2.w;

			// Perspective division of all coordinates (x,y,z,w)
			v0 /= v0.w;
			v1 /= v1.w;
			v2 /= v2.w;

			// =======================
			// Go To Pixel Coordinates
			// =======================

			v0 = viewport * v0;
			v1 = viewport * v1;
			v2 = viewport * v2;

			// Save w for perspective correct interpolation
			v0.w = v0w;
			v1.w = v1w;
			v2.w = v2w;

			cglVertices.push_back(v0);
			cglVertices.push_back(v1);
			cglVertices.push_back(v2);

			// ==============
			// Get Attributes
			// ==============

			// =================================
			// Perspective Correct Interpolation
			// =================================
			
			// UVs
			auto uv0 = cgl::vec3(model.meshes[i].vertices[j + 0].TexCoord.x, model.meshes[i].vertices[j + 0].TexCoord.y, 1.0f);
			auto uv1 = cgl::vec3(model.meshes[i].vertices[j + 1].TexCoord.x, model.meshes[i].vertices[j + 1].TexCoord.y, 1.0f);
			auto uv2 = cgl::vec3(model.meshes[i].vertices[j + 2].TexCoord.x, model.meshes[i].vertices[j + 2].TexCoord.y, 1.0f);

			auto uvPerspectiveCorrect0 = uv0 * (1 / v0.w);
			auto uvPerspectiveCorrect1 = uv1 * (1 / v1.w);
			auto uvPerspectiveCorrect2 = uv2 * (1 / v2.w);

			cglUVs.push_back(uvPerspectiveCorrect0);
			cglUVs.push_back(uvPerspectiveCorrect1);
			cglUVs.push_back(uvPerspectiveCorrect2);

			// Normals
			auto normal0 = modelView_transposed_inversed * cgl::vec4(model.meshes[i].vertices[j + 0].Normal, 1);
			auto normal1 = modelView_transposed_inversed * cgl::vec4(model.meshes[i].vertices[j + 1].Normal, 1);
			auto normal2 = modelView_transposed_inversed * cgl::vec4(model.meshes[i].vertices[j + 2].Normal, 1);

			auto normalPerspectiveCorrect0 = normal0 * (1 / v0.w);
			auto normalPerspectiveCorrect1 = normal1 * (1 / v1.w);
			auto normalPerspectiveCorrect2 = normal2 * (1 / v2.w);

			cglNormals.push_back(normalPerspectiveCorrect0);
			cglNormals.push_back(normalPerspectiveCorrect1);
			cglNormals.push_back(normalPerspectiveCorrect2);

			// Colors
			auto color0 = cgl::vec4(model.meshes[i].vertices[j + 0].Color, 1);
			auto color1 = cgl::vec4(model.meshes[i].vertices[j + 1].Color, 1);
			auto color2 = cgl::vec4(model.meshes[i].vertices[j + 2].Color, 1);

			if (shading == SHADING::GOURAUD)
			{
				auto dirLight = cgl::vec3(-m_DirectionalLight.direction).normalized();

				// vertex 0
				{
					auto diff = std::max(0.0f, dirLight.dot(normal0.to_vec3()));
					auto diffuse = m_DirectionalLight.diffuse * color0.to_vec3() * diff;
					auto ambient = m_DirectionalLight.ambient * color0.to_vec3();
					color0 = cgl::vec4(ambient + diffuse, 1.0f);
				}

				// vertex 1
				{
					auto diff = std::max(0.0f, dirLight.dot(normal1.to_vec3()));
					auto diffuse = m_DirectionalLight.diffuse * color1.to_vec3() * diff;
					auto ambient = m_DirectionalLight.ambient * color1.to_vec3();
					color1 = cgl::vec4(ambient + diffuse, 1.0f);
				}

				// vertex 2
				{
					auto diff = std::max(0.0f, dirLight.dot(normal2.to_vec3()));
					auto diffuse = m_DirectionalLight.diffuse * color2.to_vec3() * diff;
					auto ambient = m_DirectionalLight.ambient * color2.to_vec3();
					color2 = cgl::vec4(ambient + diffuse, 1.0f);
				}
			}

			auto colorPerspectiveCorrect0 = color0 * (1 / v0.w);
			auto colorPerspectiveCorrect1 = color1 * (1 / v1.w);
			auto colorPerspectiveCorrect2 = color2 * (1 / v2.w);

			cglColors.push_back(colorPerspectiveCorrect0);
			cglColors.push_back(colorPerspectiveCorrect1);
			cglColors.push_back(colorPerspectiveCorrect2);
		}
		timer_fragment_shader.reset_soft();
		Rasterize(cglVertices, cglColors, cglNormals, cglUVs);
		timer_fragment_shader.stop();
	}

	if (!m_TextureToDrawOn)
		m_TextureToDrawOn = std::make_unique<Texture>(&m_FrameBuffer.data()->r, m_screenWidth, m_screenHeight, Texture::Filtering::NEAREST_NEIGHBOR);
	else
		m_TextureToDrawOn->Update(&m_FrameBuffer.data()->r, m_screenWidth, m_screenHeight);

	if (!m_ViewportToDrawOn)
		m_ViewportToDrawOn = std::make_unique<FrameBuffer>();

	m_ViewportToDrawOn->OnRenderTexture(*m_TextureToDrawOn);
}

void Rasterizer::Rasterize(
	std::vector<cgl::vec4>& pixelCoordinates, 
	std::vector<cgl::vec4>& pixelColors, 
	std::vector<cgl::vec4>& pixelNormals,
	std::vector<cgl::vec3>& pixelUVs)
{
	for (unsigned int i = 0; i < pixelCoordinates.size(); i += 3)
	{
		auto& p0 = pixelCoordinates[i + 0];
		auto& p1 = pixelCoordinates[i + 1];
		auto& p2 = pixelCoordinates[i + 2];

		unsigned int first  = i + 0;
		unsigned int second = i + 1;
		unsigned int third  = i + 2;

		auto x0 = (unsigned int)std::round(p0.x);
		auto y0 = (unsigned int)std::round(p0.y);
		auto x1 = (unsigned int)std::round(p1.x);
		auto y1 = (unsigned int)std::round(p1.y);
		auto x2 = (unsigned int)std::round(p2.x);
		auto y2 = (unsigned int)std::round(p2.y);

		// Ordena de forma decrescente em Y (top to bottom)
		if (std::tie(y1, x1) < std::tie(y0, x0)) { std::swap(x0, x1); std::swap(y0, y1); std::swap(p0, p1); std::swap(first, second);}
		if (std::tie(y2, x2) < std::tie(y0, x0)) { std::swap(x0, x2); std::swap(y0, y2); std::swap(p0, p2); std::swap(first, third); }
		if (std::tie(y2, x2) < std::tie(y1, x1)) { std::swap(x1, x2); std::swap(y1, y2); std::swap(p1, p2); std::swap(second, third);}

		auto& c0 = pixelColors[first];
		auto& c1 = pixelColors[second];
		auto& c2 = pixelColors[third];

		auto& n0 = pixelNormals[first];
		auto& n1 = pixelNormals[second];
		auto& n2 = pixelNormals[third];

		auto& uv0 = pixelUVs[first];
		auto& uv1 = pixelUVs[second];
		auto& uv2 = pixelUVs[third];

		// triangulo não tem área. Pois y1 já está abaixo de y0, então se y0 == y2, eles estão todos juntos
		/*if (y0 == y2)
			return;*/

		// Determina se o lado menor está na esquerda ou direita
		// verdadeiro = direito
		// falso = esquerdo
		bool shortside = (y1 - y0) * (x2 - x0) < (x1 - x0) * (y2 - y0);

		// Criamos 2 retas: p0-p1 (menor) e p0-p2(maior)
		std::array<std::unique_ptr<Slope<float>>, 2> slope_x;
		std::array<std::unique_ptr<Slope<float>>, 2> slope_z;

		std::array<std::unique_ptr<Slope<cgl::vec4>>, 2> slope_color;
		std::array<std::unique_ptr<Slope<cgl::vec4>>, 2> slope_normal;
		std::array<std::unique_ptr<Slope<cgl::vec3>>, 2> slope_uv;

		int n_steps_longside = p2.y - p0.y;

		slope_x[!shortside] = std::make_unique<Slope<float>>(p0.x, p2.x, n_steps_longside);
		slope_z[!shortside] = std::make_unique<Slope<float>>(p0.z, p2.z, n_steps_longside);

		slope_color[!shortside]  = std::make_unique<Slope<cgl::vec4>>(c0, c2, n_steps_longside);
		slope_normal[!shortside] = std::make_unique<Slope<cgl::vec4>>(n0, n2, n_steps_longside);
		slope_uv[!shortside]     = std::make_unique<Slope<cgl::vec3>>(uv0, uv2, n_steps_longside);

		// ====================
		// Main Rasterizer Loop
		// ====================

		// Check if not y0 == y1
		if (y0 < y1)
		{
			// Calcula a segunda reta para o lado menor
			int n_steps = p1.y - p0.y;

			slope_x[shortside] = std::make_unique<Slope<float>>(p0.x, p1.x, n_steps);
			slope_z[shortside] = std::make_unique<Slope<float>>(p0.z, p1.z, n_steps);

			slope_color[shortside]  = std::make_unique<Slope<cgl::vec4>>(c0,  c1,  n_steps );
			slope_normal[shortside] = std::make_unique<Slope<cgl::vec4>>(n0,  n1,  n_steps);
			slope_uv[shortside]     = std::make_unique<Slope<cgl::vec3>>(uv0, uv1, n_steps);

			for (auto y = y0; y < y1; ++y)
			{
				Scanline(y,
					std::round(slope_x[0]->get()), std::round(slope_x[1]->get()),
					slope_z[0]->get(), slope_z[1]->get(),
					slope_color[0]->get(), slope_color[1]->get(),
					slope_normal[0]->get(), slope_normal[1]->get(),
					slope_uv[0]->get(), slope_uv[1]->get());

				slope_x[0]->advance();
				slope_x[1]->advance();
				slope_z[0]->advance();
				slope_z[1]->advance();

				slope_color[0]->advance();
				slope_color[1]->advance();

				slope_normal[0]->advance();
				slope_normal[1]->advance();

				slope_uv[0]->advance();
				slope_uv[1]->advance();
			}
		}

		// Check if not y1 == y2
		if (y1 < y2)
		{
			// Calcula a terceira reta
			int n_steps = p2.y - p1.y;

			slope_x[shortside] = std::make_unique<Slope<float>>(p1.x, p2.x, n_steps);
			slope_z[shortside] = std::make_unique<Slope<float>>(p1.z, p2.z, n_steps);

			slope_color[shortside]  = std::make_unique<Slope<cgl::vec4>>(c1,  c2,  n_steps);
			slope_normal[shortside] = std::make_unique<Slope<cgl::vec4>>(n1,  n2,  n_steps);
			slope_uv[shortside]     = std::make_unique<Slope<cgl::vec3>>(uv1, uv2, n_steps);

			for (auto y = y1; y < y2; ++y)
			{
				Scanline(y,
					std::round(slope_x[0]->get()), std::round(slope_x[1]->get()),
					slope_z[0]->get(),      slope_z[1]->get(),
					slope_color[0]->get(),  slope_color[1]->get(),
					slope_normal[0]->get(), slope_normal[1]->get(),
					slope_uv[0]->get(),     slope_uv[1]->get());

				slope_x[0]->advance();
				slope_x[1]->advance();
				slope_z[0]->advance();
				slope_z[1]->advance();

				slope_color[0]->advance();
				slope_color[1]->advance();

				slope_normal[0]->advance();
				slope_normal[1]->advance();

				slope_uv[0]->advance();
				slope_uv[1]->advance();
			}
		}
	}
}

void Rasterizer::Scanline(
	unsigned int y, 
	int x_left, int x_right,
	float z_left, float z_right,
	cgl::vec4 color_left, cgl::vec4 color_right, 
	cgl::vec4 normal_left, cgl::vec4 normal_right,
	cgl::vec3 uv_left, cgl::vec3 uv_right)
{
	// TODO: why????
	if (x_right < x_left)
	{
		std::swap(x_right, x_left);
		std::swap(z_right, z_left);
		std::swap(color_right, color_left);
		std::swap(normal_right, normal_left);
		std::swap(uv_left, uv_right);
	}

	Slope<float> z_buf     (z_left, z_right,           x_right - x_left);
	Slope<cgl::vec4> color (color_left, color_right,   x_right - x_left);
	Slope<cgl::vec4> normal(normal_left, normal_right, x_right - x_left);
	Slope<cgl::vec3> uv    (uv_left, uv_right,         x_right - x_left);

	if (m_Primitive == PRIMITIVE::Triangle)
	{
		for (int x = x_left; x < x_right; ++x)
		{
			if (z_buf.get() < m_ZBuffer.get(m_ZBuffer.height() - 1 - y, x))
			{
				cgl::vec3 pixelColor  = (color.get()  * (1 / color.get().w)).to_vec3();
				cgl::vec3 pixelNormal = (normal.get() * (1 / normal.get().w)).to_vec3().normalized();
				cgl::vec2 pixelUV     = (uv.get()     * (1 / uv.get().z)).to_vec2();


				if (m_ShowTexture && m_CurrentTexture)
				{
					const unsigned char* const textureBuffer = m_CurrentTexture->GetLocalBuffer();
					pixelColor = { 0.0f,0.0f,0.0f };

					if (m_Filtering == Texture::Filtering::NEAREST_NEIGHBOR)
					{
						unsigned int u = std::floor(std::clamp(pixelUV.x, 0.0f, 1.0f) * (float)(m_CurrentTexture->GetWidth()  - 1.0f));
						unsigned int v = std::floor(std::clamp(pixelUV.y, 0.0f, 1.0f) * (float)(m_CurrentTexture->GetHeight() - 1.0f));

						pixelColor = Texture::GetPixelColorFromTextureBuffer(textureBuffer, m_CurrentTexture->GetWidth(), u, v);
					}

					else if (m_Filtering == Texture::Filtering::BILINEAR)
					{
						float u = std::clamp(pixelUV.x, 0.0f, 1.0f) * (float)(m_CurrentTexture->GetWidth()  - 1.0f);
						float v = std::clamp(pixelUV.y, 0.0f, 1.0f) * (float)(m_CurrentTexture->GetHeight() - 1.0f);

						pixelColor = Texture::BilinearFiltering(textureBuffer, m_CurrentTexture->GetWidth(), u, v);
					}

					else if (m_Filtering == Texture::Filtering::BICUBIC)
					{
						float u = std::clamp(pixelUV.x, 0.0f, 1.0f) * (float)(m_CurrentTexture->GetWidth() - 1.0f);
						float v = std::clamp(pixelUV.y, 0.0f, 1.0f) * (float)(m_CurrentTexture->GetHeight() - 1.0f);

						pixelColor = Texture::BicubicFiltering(textureBuffer, m_CurrentTexture->GetWidth(), m_CurrentTexture->GetHeight(), u, v);
					}

					else if (m_Filtering == Texture::Filtering::TRILLINEAR)
					{
						float u = std::clamp(pixelUV.x, 0.0f, 1.0f);
						float v = std::clamp(pixelUV.y, 0.0f, 1.0f);

						auto st = uv;
						st.advance();
						auto pixelST = (st.get() * (1 / st.get().z)).to_vec2();

						float next_s = std::clamp(pixelST.x, 0.0f, 1.0f);
						float next_t = std::clamp(pixelST.y, 0.0f, 1.0f);

						auto ds = (next_s - u) * m_CurrentTexture->GetWidth();
						auto dt = (next_t - v) * m_CurrentTexture->GetHeight();

						float mipmap_level = std::abs(MipMap::GetMipMapLevel(ds, dt));
						mipmap_level = std::clamp(mipmap_level, 0.0f, 6.0f);

						const auto mipmap = m_CurrentTexture->GetMipMap();

						unsigned char* mipmaps_levels[2];

						float t = (mipmap_level - std::floor(mipmap_level));

						auto level_0 = std::floor(mipmap_level);
						auto level_1 = std::ceil(mipmap_level);

						float width_0 = m_CurrentTexture->GetWidth() / (std::pow(2,level_0));
						float width_1 = m_CurrentTexture->GetWidth() / (std::pow(2,level_1));

						float height_0 = m_CurrentTexture->GetHeight() / (std::pow(2, level_0));
						float height_1 = m_CurrentTexture->GetHeight() / (std::pow(2, level_1));

						mipmaps_levels[0] = mipmap->GetLevel(level_0);
						mipmaps_levels[1] = mipmap->GetLevel(level_1);

						auto color0 = Texture::BilinearFiltering(mipmaps_levels[0], width_0, u * width_0, v * height_0);
						auto color1 = Texture::BilinearFiltering(mipmaps_levels[1], width_1, u * width_1, v * height_1);

						pixelColor = (1.0f - t) * color0 + (t) * color1;
					}
				}

				if (m_Shading == SHADING::PHONG)
				{
					auto dirLight = cgl::vec3(-m_DirectionalLight.direction).normalized();
					auto diff = std::max(0.0f, dirLight.dot(pixelNormal));
					auto diffuse = m_DirectionalLight.diffuse * pixelColor * diff;

					auto ambient = m_DirectionalLight.ambient * pixelColor;

					pixelColor = ambient + diffuse;
				}

				// else if (m_Shading == SHADING::NONE)

				m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x, to_pixel(pixelColor));
				m_ZBuffer.set(m_ZBuffer.height() - 1 - y, x, z_buf.get());

				z_buf.advance();
				color.advance();
				normal.advance();
				uv.advance();
			}
		}
	}

	else if (m_Primitive == PRIMITIVE::WireFrame)
	{
		if (z_buf.get() < m_ZBuffer.get(m_ZBuffer.height() - 1 - y, x_left))
			m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x_left, to_pixel(color_left.to_vec3()));
		if (z_buf.get() < m_ZBuffer.get(m_ZBuffer.height() - 1 - y, x_right))
			m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x_right, to_pixel(color_right.to_vec3()));
	}

	else if (m_Primitive == PRIMITIVE::Point)
	{
		if (z_buf.get() < m_ZBuffer.get(m_ZBuffer.height() - 1 - y, x_left))
			m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x_left, to_pixel(color_left.to_vec3()));
		if (z_buf.get() < m_ZBuffer.get(m_ZBuffer.height() - 1 - y, x_right))
			m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x_right, to_pixel(color_right.to_vec3()));
	}
}

