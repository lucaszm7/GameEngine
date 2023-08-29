#pragma once

#include "camera.h"
#include "physics/SphereCollider.h"

struct Player
{
	std::shared_ptr<ogl::Camera> camera;

	Collider::Sphere collider;

	Player(std::shared_ptr<ogl::Camera> cam);
	
	glm::vec3 acc;

	void OnPhysics(Collider::Info& collInfo);
};

