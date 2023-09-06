#pragma once

#include "glm/glm.hpp"
#include "model.h"

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

		glm::vec3 normal() const
		{
			return glm::normalize(glm::cross(v1 - v0, v2 - v0));
		}
	};
	
	glm::vec3 CheckCollision(const Sphere& a, const Sphere& b);
	Info CheckCollision(const Sphere& s, const Triangle& t);
	glm::vec3 ClosestPointToTriangle(const glm::vec3& P, const Triangle& t);
	std::vector<Collider::Info> CheckCollision(const Sphere& s, const Model& t);
}