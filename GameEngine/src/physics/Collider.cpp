#include "Collider.h"

glm::vec3 Collider::CheckCollision(const Sphere& a, const Sphere& b)
{
	auto radius = (a.radius + b.radius);
	auto lenght = glm::length(a.center - b.center);
	if (radius < lenght)
		return glm::vec3(0.0f);

	return glm::normalize(b.center - a.center) * (radius - lenght);
}

glm::vec3 Collider::ClosestPointToTriangle(const glm::vec3& P, const Triangle& t)
{
	glm::vec3 N = t.normal();
	auto& V0 = t.v0;
	auto& V1 = t.v1;
	auto& V2 = t.v2;

	glm::vec3 CP = P - N * glm::dot(P - V0, N); // projected sphere center on triangle plane

	// Now determine whether CP is inside all triangle edges: 
	auto u = glm::dot((V1 - V0), (CP - V0)) / glm::dot((V1 - V0), (V1 - V0));
	auto v = glm::dot((V2 - V0), (CP - V0)) / glm::dot((V2 - V0), (V2 - V0));
	auto w = 1 - u - v;

	bool is_u = (0.0f <= u) && (u <= 1.0f);
	bool is_v = (0.0f <= v) && (v <= 1.0f);
	bool is_w = (0.0f <= w) && (w <= 1.0f);

	bool inside = is_u && is_v && is_w;
	if (inside)
		return CP;

	// If CP is outside the triangle, clamp it to the nearest edge or vertex
	if (u < 0.0f)
		return V0;
	if (v < 0.0f)
		return V1;
	if (w < 0.0f)
		return V2;

	float d0 = glm::distance(P, V0);
	float d1 = glm::distance(P, V1);
	float d2 = glm::distance(P, V2);

	if (d0 <= d1 && d0 <= d2)
		return V0;
	if (d1 <= d0 && d1 <= d2)
		return V1;

	return V2;
}

Collider::Info Collider::CheckCollision(const Sphere& s, const Triangle& t)
{
	glm::vec3 closestPoint = ClosestPointToTriangle(s.center, t);
	float distanceToCP = glm::distance(s.center, closestPoint);

	bool collision = distanceToCP < s.radius;
	if (collision)
	{
		float penetrationDepth = s.radius - distanceToCP;

		glm::vec3 collisionNormal = t.normal();

		return { true, closestPoint, collisionNormal, penetrationDepth };
	}

	else
		return { false };
}

std::vector<Collider::Info> Collider::CheckCollision(const Sphere& s, const Model& model)
{
	std::vector<Collider::Info> info;

	for (auto& mesh : model.meshes)
	{
		for (unsigned int i = 0; i < mesh.vertices.size() - 2; i += 3)
		{
			auto& v0 = mesh.vertices[mesh.indices[i] + 0].Position;
			auto& v1 = mesh.vertices[mesh.indices[i] + 1].Position;
			auto& v2 = mesh.vertices[mesh.indices[i] + 2].Position;
			Triangle tri{ v0, v1, v2 };
			auto inf = CheckCollision(s, tri);
			if (!inf.collision)
				continue;
			info.push_back(inf);
		}
	}
	return info;
}
