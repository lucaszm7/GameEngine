#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "mesh.h"
#include "mat.hpp"
#include "mat4.h"
#include "vec4.h"
#include "ViewPort.hpp"
#include "model.h"
#include "camera.h"


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

struct Slope
{
	float curr;
	float step;

	// starting position + distance between start & end * how far we have traveled / how far we are going to travel
	// x_start + (x_end - x_start) * [(y - y_start) / (y_end - y_start)]
	// the multiplication gives the t value (percentage traveled)
	// For this Y where is the X?
	Slope(float begin, float end, int n_steps)
	{
		float inv_steps = 1.0f / n_steps;
		step = (end - begin) * inv_steps;
		curr = begin;
	}

	float get() const { return curr; }
	void advance() { curr += step; }
};

class Rasterizer 
{
public:
	static void DrawSoftwareRasterized(
		const Model& model, const cgl::Camera& camera, const ViewPort& drawTextureOn,
		unsigned int screenWidth,
		unsigned int screenHeight,
		DrawPrimitive drawPrimitive = DrawPrimitive::Triangle,
		bool isCulling = false,
		bool isCullingClockWise = false);

	static void SetViewPort(const unsigned int screenWidth, const unsigned int screenHeight);
	static void ClearFrameBuffer();
	static void SetClearColor(const Pixel& p) { m_ClearColor = p; };
	static void ClearZBuffer();
	static cgl::mat<Pixel>* GetFrameBuffer() { return &m_FrameBuffer; };

private:
	Rasterizer();
	Rasterizer(const Rasterizer&);
	static void Rasterize(std::vector<cgl::vec4>& pixelCoordinates, std::vector<Pixel>& pixelColors, DrawPrimitive drawPrimitive);
	static void Scanline(unsigned int y, Slope& left, Slope& right, Pixel color, DrawPrimitive drawPrimitive);

	inline static Pixel m_ClearColor = Pixel{ 255,255,255 };
	inline static cgl::mat<Pixel> m_FrameBuffer;
	inline static cgl::mat<float> m_ZBuffer;
};