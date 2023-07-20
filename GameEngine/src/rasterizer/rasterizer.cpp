#include "rasterizer.hpp"

static Pixel to_pixel(const glm::vec3& c)
{
	return { (unsigned char)(c.x * 255), (unsigned char)(c.y * 255), (unsigned char)(c.z * 255) };
}

void Rasterizer::DrawSoftwareRasterized(
	const Model& model, 
	const cgl::Camera& camera, 
	const ViewPort& drawTextureOn,
	unsigned int screenWidth,
	unsigned int screenHeight,
	DrawPrimitive drawPrimitive,
	bool isCulling,
	bool isCullingClockWise)
{
	// Build Model Matrix
	cgl::mat4 translate = cgl::mat4::translate(cgl::vec4(model.transform.position, 1.0f));
	// model = model * cgl::mat4::rotateX(transform.rotation.x);
	// model = model * cgl::mat4::rotateY(transform.rotation.y);
	// model = model * cgl::mat4::rotateZ(transform.rotation.z);
	cgl::mat4 scale = cgl::mat4::scale(model.transform.scale);
	cgl::mat4 modelM = translate * scale;

	// Build View Matrix
	cgl::mat4 view = camera.GetViewMatrix();

	// Build Projection Matrix
	cgl::mat4 projection = camera.GetProjectionMatrix((float)screenWidth / (float)screenHeight);

	// Buil ViewPort Matrix
	cgl::mat4 viewport = cgl::mat4::viewport(screenWidth, screenHeight);

	// ================
	// Build MVP Matrix
	// ================
	cgl::mat4 mvp = projection.transpose() * view.transpose() * modelM;

	for (unsigned int i = 0; i < model.meshes.size(); ++i)
	{
		std::vector<cgl::vec4> cglVertices;
		std::vector<Pixel> cglColors;

		cglVertices.reserve(model.meshes[i].vertices.size());

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

			// Perspective division
			v0 /= v0.w;
			v1 /= v1.w;
			v2 /= v2.w;

			// =======================
			// Go To Pixel Coordinates
			// =======================

			v0 = viewport * v0;
			v1 = viewport * v1;
			v2 = viewport * v2;

			cglVertices.push_back(v0);
			cglVertices.push_back(v1);
			cglVertices.push_back(v2);

			// Get vertices colors
			cglColors.push_back(to_pixel(model.meshes[i].vertices[j + 0].Color));
			cglColors.push_back(to_pixel(model.meshes[i].vertices[j + 1].Color));
			cglColors.push_back(to_pixel(model.meshes[i].vertices[j + 2].Color));
		}
		Rasterize(cglVertices, cglColors, drawPrimitive);
	}
	Texture texCGL(&m_FrameBuffer.data()->r, screenWidth, screenHeight);
	drawTextureOn.OnRenderTexture(texCGL);
}

void Rasterizer::SetViewPort(const unsigned int screenWidth, const unsigned int screenHeight)
{
	m_FrameBuffer.resize(screenHeight, screenWidth);
	m_ZBuffer.resize(screenHeight, screenWidth);
}

void Rasterizer::ClearFrameBuffer()
{
	m_FrameBuffer.clear(m_ClearColor);
}

void Rasterizer::ClearZBuffer()
{
	m_ZBuffer.clear(std::numeric_limits<float>::max());
}

void Rasterizer::Scanline(unsigned int y, 
	int x_left, int x_right,
	float z_left, float z_right,
	Pixel& color_left, Pixel& color_right, 
	DrawPrimitive drawPrimitive)
{
	// TODO: why????
	if (x_right < x_left)
	{
		std::swap(x_right, x_left);
		std::swap(z_right, z_left);
		std::swap(color_right, color_left);
	}

	Slope z_buf(z_left, z_right, x_right - x_left);
	Slope r(color_left.r, color_right.r, x_right - x_left);
	Slope g(color_left.g, color_right.g, x_right - x_left);
	Slope b(color_left.b, color_right.b, x_right - x_left);

	if (drawPrimitive == DrawPrimitive::Triangle)
	{
		for (int x = x_left; x < x_right; ++x)
		{
			if (z_buf.get() < m_ZBuffer.get(m_ZBuffer.height() - 1 - y, x))
			{
				m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x, {(unsigned char)r.get(), (unsigned char)g.get(), (unsigned char)b.get()});
				m_ZBuffer.set(m_ZBuffer.height() - 1 - y, x, z_buf.get());
				z_buf.advance();
				r.advance(); g.advance(); b.advance();
			}
		}
	}
	else if (drawPrimitive == DrawPrimitive::WireFrame)
	{
		m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x_left, { (unsigned char)r.get(), (unsigned char)g.get(), (unsigned char)b.get() });
		m_FrameBuffer.set(m_FrameBuffer.height() - 1 - y, x_right, { (unsigned char)r.get(), (unsigned char)g.get(), (unsigned char)b.get() });
	}
}

void Rasterizer::Rasterize(std::vector<cgl::vec4>& pixelCoordinates, std::vector<Pixel>& pixelColors, DrawPrimitive drawPrimitive)
{
	for (unsigned int i = 0; i < pixelCoordinates.size(); i += 3)
	{
		auto& p0 = pixelCoordinates[i + 0];
		auto& p1 = pixelCoordinates[i + 1];
		auto& p2 = pixelCoordinates[i + 2];

		auto& c0 = pixelColors[i + 0];
		auto& c1 = pixelColors[i + 1];
		auto& c2 = pixelColors[i + 2];

		auto x0 = (unsigned int)std::trunc(p0.x);
		auto y0 = (unsigned int)std::trunc(p0.y);
		auto x1 = (unsigned int)std::trunc(p1.x);
		auto y1 = (unsigned int)std::trunc(p1.y);
		auto x2 = (unsigned int)std::trunc(p2.x);
		auto y2 = (unsigned int)std::trunc(p2.y);

		// Ordena de forma decrescente em Y (top to bottom)
		if (std::tie(y1, x1) < std::tie(y0, x0)) { std::swap(x0, x1); std::swap(y0, y1); std::swap(p0, p1); std::swap(c0, c1); }
		if (std::tie(y2, x2) < std::tie(y0, x0)) { std::swap(x0, x2); std::swap(y0, y2); std::swap(p0, p2); std::swap(c0, c2); }
		if (std::tie(y2, x2) < std::tie(y1, x1)) { std::swap(x1, x2); std::swap(y1, y2); std::swap(p1, p2); std::swap(c1, c2); }

		// triangulo não tem área. Pois y1 já está abaixo de y0, então se y0 == y2, eles estão todos juntos
		/*if (y0 == y2)
			return;*/

		// Determina se o lado menor está na esquerda ou direita
		// verdadeiro = direito
		// falso = esquerdo
		bool shortside = (y1 - y0) * (x2 - x0) < (x1 - x0) * (y2 - y0);

		// Criamos 2 retas: p0-p1 (menor) e p0-p2(maior)
		std::array<std::unique_ptr<Slope>, 2> slope_x;
		std::array<std::unique_ptr<Slope>, 2> slope_z;

		std::array<std::unique_ptr<Slope>, 2> slope_r;
		std::array<std::unique_ptr<Slope>, 2> slope_g;
		std::array<std::unique_ptr<Slope>, 2> slope_b;

		slope_x[!shortside] = std::make_unique<Slope>(p0.x, p2.x, p2.y - p0.y);
		slope_z[!shortside] = std::make_unique<Slope>(p0.z, p2.z, p2.y - p0.y);

		slope_r[!shortside] = std::make_unique<Slope>(c0.r, c2.r, p2.y - p0.y);
		slope_g[!shortside] = std::make_unique<Slope>(c0.g, c2.g, p2.y - p0.y);
		slope_b[!shortside] = std::make_unique<Slope>(c0.b, c2.b, p2.y - p0.y);

		// ====================
		// Main Rasterizer Loop
		// ====================

		// Check if not y0 == y1
		if (y0 < y1)
		{
			slope_x[shortside] = std::make_unique<Slope>(p0.x, p1.x, p1.y - p0.y);
			slope_z[shortside] = std::make_unique<Slope>(p0.z, p1.z, p1.y - p0.y);

			slope_r[shortside] = std::make_unique<Slope>(c0.r, c1.r, p2.y - p0.y);
			slope_g[shortside] = std::make_unique<Slope>(c0.g, c1.g, p2.y - p0.y);
			slope_b[shortside] = std::make_unique<Slope>(c0.b, c1.b, p2.y - p0.y);

			for (auto y = y0; y < y1; ++y)
			{
				Pixel color_left = { (unsigned char)slope_r[0]->get(), (unsigned char)slope_g[0]->get(), (unsigned char)slope_b[0]->get() };
				Pixel color_right = { (unsigned char)slope_r[1]->get(), (unsigned char)slope_g[1]->get(), (unsigned char)slope_b[1]->get() };

				Scanline(y, 
					slope_x[0]->get(), slope_x[1]->get(), 
					slope_z[0]->get(), slope_z[1]->get(), 
					color_left, color_right,
					drawPrimitive);
				
				slope_x[0]->advance();
				slope_x[1]->advance();
				slope_z[0]->advance();
				slope_z[1]->advance();

				slope_r[0]->advance();
				slope_r[1]->advance();
				slope_g[0]->advance();
				slope_g[1]->advance();
				slope_b[0]->advance();
				slope_b[1]->advance();
			}
		}

		// Check if not y1 == y2
		if (y1 < y2)
		{
			// Calcula a segunda reta para o lado menor
			slope_x[shortside] = std::make_unique<Slope>(p1.x, p2.x, p2.y - p1.y);
			slope_z[shortside] = std::make_unique<Slope>(p1.z, p2.z, p2.y - p1.y);

			slope_r[shortside] = std::make_unique<Slope>(c1.r, c2.r, p2.y - p1.y);
			slope_g[shortside] = std::make_unique<Slope>(c1.g, c2.g, p2.y - p1.y);
			slope_b[shortside] = std::make_unique<Slope>(c1.b, c2.b, p2.y - p1.y);

			for (auto y = y1; y < y2; ++y)
			{
				Pixel color_left = { (unsigned char)slope_r[0]->get(), (unsigned char)slope_g[0]->get(), (unsigned char)slope_b[0]->get() };
				Pixel color_right = { (unsigned char)slope_r[1]->get(), (unsigned char)slope_g[1]->get(), (unsigned char)slope_b[1]->get() };

				Scanline(y,
					slope_x[0]->get(), slope_x[1]->get(),
					slope_z[0]->get(), slope_z[1]->get(),
					color_left, color_right,
					drawPrimitive);

				slope_x[0]->advance();
				slope_x[1]->advance();
				slope_z[0]->advance();
				slope_z[1]->advance();

				slope_r[0]->advance();
				slope_r[1]->advance();
				slope_g[0]->advance();
				slope_g[1]->advance();
				slope_b[0]->advance();
				slope_b[1]->advance();
			}
		}
	}
}