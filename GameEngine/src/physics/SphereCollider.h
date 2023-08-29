#pragma once

#include "glm/glm.hpp"
#include "mesh.h"

namespace Collider
{
	struct Info
	{
		bool collision;
		glm::vec3 inter;
		glm::vec3 normal;
		float depth;
	};

	struct Sphere
	{
		glm::vec3 center;
		float radius;
		
	};

	struct Triangle
	{
		glm::vec3 v0, v1, v2;
	};
	
	glm::vec3 CheckCollision(const Sphere& a, const Sphere& b);
	Info CheckCollision(const Sphere& s, const Triangle& t);

	std::vector<Collider::Info> CheckCollision(const Sphere& s, const std::vector<Mesh>& t);
}