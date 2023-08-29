#include "SphereCollider.h"

glm::vec3 Collider::CheckCollision(const Sphere& a, const Sphere& b)
{
	auto radius = (a.radius + b.radius);
	auto lenght = glm::length(a.center - b.center);
	if (radius < lenght)
		return glm::vec3(0.0f);

	return glm::normalize(b.center - a.center) * (radius - lenght);
}

Collider::Info Collider::CheckCollision(const Sphere& s, const Triangle& t)
{
	glm::vec3 N = glm::normalize(glm::cross(t.v1 - t.v0, t.v2 - t.v0));
	float dist = glm::dot(t.v0 - s.center, N);
	
	if (dist < -s.radius || dist > s.radius)
		return { false };

	glm::vec3 point0 = s.center - (N * dist); // projected sphere center on triangle plane

	// Now determine whether point0 is inside all triangle edges: 
	glm::vec3 c0 = glm::cross(point0 - t.v0, t.v1 - t.v0);
	glm::vec3 c1 = glm::cross(point0 - t.v1, t.v2 - t.v1);
	glm::vec3 c2 = glm::cross(point0 - t.v2, t.v0 - t.v2);
	bool inside = dot(c0, N) <= 0 && dot(c1, N) <= 0 && dot(c2, N) <= 0;

	constexpr auto ClosestPointOnLineSegment = [](glm::vec3 A, glm::vec3 B, glm::vec3 Point)
		{
			glm::vec3 AB = B - A;
			float t = glm::dot(Point - A, AB) / dot(AB, AB);
			return A + std::min(std::max(t, 0.0f), 1.0f) * AB; // saturate(t) can be written as: min((max(t, 0), 1)
		};

	float radiussq = s.radius * s.radius; // sphere radius squared

	// Edge 1:
	glm::vec3 point1 = ClosestPointOnLineSegment(t.v0, t.v1, s.center);
	glm::vec3 v1 = s.center - point1;
	float distsq1 = dot(v1, v1);
	bool intersects = distsq1 < radiussq;

	// Edge 2:
	glm::vec3 point2 = ClosestPointOnLineSegment(t.v1, t.v2, s.center);
	glm::vec3 v2 = s.center - point2;
	float distsq2 = glm::dot(v2, v2);
	intersects |= distsq2 < radiussq;

	// Edge 3:
	glm::vec3 point3 = ClosestPointOnLineSegment(t.v2, t.v0, s.center);
	glm::vec3 v3 = s.center - point3;
	float distsq3 = glm::dot(v3, v3);
	intersects |= distsq3 < radiussq;

	if (!inside && !intersects)
		return { false };

	glm::vec3 best_point = point0;
	glm::vec3 intersection_vec;

	if (inside)
	{
		intersection_vec = s.center - point0;
	}
	else
	{
		glm::vec3 d = s.center - point1;
		float best_distsq = dot(d, d);
		best_point = point1;
		intersection_vec = d;

		d = s.center - point2;
		float distsq = dot(d, d);
		if (distsq < best_distsq)
		{
			distsq = best_distsq;
			best_point = point2;
			intersection_vec = d;
		}

		d = s.center - point3;
		distsq = dot(d, d);
		if (distsq < best_distsq)
		{
			distsq = best_distsq;
			best_point = point3;
			intersection_vec = d;
		}
	}

	float len = glm::length(intersection_vec);  // vector3 length calculation: std::sqrt(glm::dot(v, v))
	glm::vec3 penetration_normal = intersection_vec / len;  // normalize
	float penetration_depth = s.radius - len; // radius = sphere radius
	return { true, best_point, penetration_normal, penetration_depth }; // intersection success
}

std::vector<Collider::Info> Collider::CheckCollision(const Sphere& s, const std::vector<Mesh>& meshes)
{
	std::vector<Collider::Info> info;

	for (auto& mesh : meshes)
	{
		for (unsigned int i = 0; i < mesh.vertices.size() - 2; i += 3)
		{
			Triangle tri{ mesh.vertices[i + 0].Position, mesh.vertices[i + 1].Position, mesh.vertices[i + 2].Position };
			auto inf = CheckCollision(s, tri);
			if (!inf.collision)
				continue;
			info.push_back(inf);
		}
	}
	return info;
}
