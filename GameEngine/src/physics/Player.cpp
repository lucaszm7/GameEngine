#include "Player.h"

Player::Player(std::shared_ptr<ogl::Camera> cam)
	:camera(cam), collider{camera->Position, 1.0f}, acc(0) {}

void Player::OnPhysics(Collider::Info& collInfo)
{
	auto& normal = collInfo.normal;
	auto& depth = collInfo.depth;

	camera->Position += normal * depth;
	collider.center = camera->Position;
}

