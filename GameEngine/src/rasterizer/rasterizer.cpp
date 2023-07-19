#include "rasterizer.hpp"


void Rasterizer::DrawSoftwareRasterized(
	const Model& model, const cgl::Camera& camera, const ViewPort& drawTextureOn,
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
	cgl::mat4 projection = camera.GetProjectionMatrix(screenWidth / screenHeight);

	// Buil ViewPort Matrix
	cgl::mat4 viewport = cgl::mat4::viewport(screenWidth, screenHeight);

	// ================
	// Build MVP Matrix
	// ================
	cgl::mat4 mvp = projection.transpose() * view.transpose() * modelM;

	for (unsigned int i = 0; i < model.meshes.size(); ++i)
	{
		std::vector<cgl::vec4> cglVertices;
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
		}
		Rasterize(cglVertices, drawPrimitive);
	}
	Texture texCGL(&m_FrameBuffer.data()->r, screenWidth, screenHeight);
	drawTextureOn.OnRenderTexture(texCGL);
}

void Rasterizer::SetViewPort(const unsigned int screenWidth, const unsigned int screenHeight)
{
	m_FrameBuffer.resize(screenWidth, screenHeight);
	m_ZBuffer.resize(screenWidth, screenHeight);
}

void Rasterizer::ClearFrameBuffer()
{
	m_FrameBuffer.clear(m_ClearColor);
}

void Rasterizer::ClearZBuffer()
{
	m_ZBuffer.clear(std::numeric_limits<float>::max());
}

void Rasterizer::Scanline(unsigned int y, Slope& left, Slope& right, DrawPrimitive drawPrimitive)
{
	auto x_left  = (int)left.get();
	auto x_right = (int)right.get();

	// TODO: why????
	if (x_right < x_left)
		std::swap(x_right, x_left);

	if (drawPrimitive == DrawPrimitive::WireFrame)
	{
		m_FrameBuffer.set(y, x_left, { 255, 0, 0 });
		m_FrameBuffer.set(y, x_right, { 255, 0, 0 });
	}
	else
	{
		for (int x = x_left; x < x_right; ++x)
		{
			m_FrameBuffer.set(y, x, { 255, 0, 0 });
		}
	}

	left.advance();
	right.advance();
}

void Rasterizer::Rasterize(std::vector<cgl::vec4>& pixelCoordinates, DrawPrimitive drawPrimitive)
{

	for (unsigned int i = 0; i < pixelCoordinates.size(); i += 3)
	{
		auto& p0 = pixelCoordinates[i + 0];
		auto& p1 = pixelCoordinates[i + 1];
		auto& p2 = pixelCoordinates[i + 2];

		auto x0 = (unsigned int)std::trunc(p0.x);
		auto y0 = (unsigned int)std::trunc(p0.y);
		auto x1 = (unsigned int)std::trunc(p1.x);
		auto y1 = (unsigned int)std::trunc(p1.y);
		auto x2 = (unsigned int)std::trunc(p2.x);
		auto y2 = (unsigned int)std::trunc(p2.y);

		// Ordena de forma decrescente em Y (top to bottom)
		if (std::tie(y1, x1) < std::tie(y0, x0)) { std::swap(x0, x1); std::swap(y0, y1); std::swap(p0, p1); }
		if (std::tie(y2, x2) < std::tie(y0, x0)) { std::swap(x0, x2); std::swap(y0, y2); std::swap(p0, p2); }
		if (std::tie(y2, x2) < std::tie(y1, x1)) { std::swap(x1, x2); std::swap(y1, y2); std::swap(p1, p2); }

		// triangulo não tem área. Pois y1 já está abaixo de y0, então se y0 == y2, eles estão todos juntos
		if (y0 == y2)
			return;

		// Determina se o lado menor está na esquerda ou direita
		// verdadeiro = direito
		// falso = esquerdo
		bool shortside = (y1 - y0) * (x2 - x0) < (x1 - x0) * (y2 - y0);

		// Criamos 2 retas: p0-p1 (menor) e p0-p2(maior)
		std::array<std::unique_ptr<Slope>, 2> sides;
		sides[!shortside] = std::make_unique<Slope>(p0.x, p2.x, p2.y - p0.y);

		// ====================
		// Main Rasterizer Loop
		// ====================

		// Check if not y0 == y1
		if (y0 < y1)
		{
			sides[shortside] = std::make_unique<Slope>(p0.x, p1.x, p1.y - p0.y);
			for (auto y = y0; y < y1; ++y)
			{
				Scanline(y, *sides[0], *sides[1], drawPrimitive);
			}
		}

		// Check if not y1 == y2
		if (y1 < y2)
		{
			// Calcula a segunda reta para o lado menor
			sides[shortside] = std::make_unique<Slope>(p1.x, p2.x, p2.y - p1.y);
			for (auto y = y1; y < y2; ++y)
			{
				Scanline(y, *sides[0], *sides[1], drawPrimitive);
			}
		}
	}
}