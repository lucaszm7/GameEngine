#include "Player.h"

Player::Player(std::shared_ptr<ogl::Camera> cam)
	:camera(cam), collider{camera->Position, 1.0f}, acc(0)
{

}

void Player::OnPhysics(Collider::Info& collInfo)
{
	auto& normal = collInfo.normal;
	auto& depth = collInfo.depth;
	// auto& velocity = camera->velocity;

	camera->Position += normal * depth;

	// camera->velocity = velocity - 2.0f * glm::dot(velocity, normal) * normal;

	collider.center = camera->Position;
}

