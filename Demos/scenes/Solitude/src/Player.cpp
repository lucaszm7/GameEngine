#include "Player.h"

Player::Player(std::shared_ptr<ogl::Camera> cam)
	:camera(cam), collider{camera->Position, 1.0f}, acc(0)
{

}

void Player::OnPhysics(Collider::Info& collInfo)
{
	// Modify player velocity to slide on contact surface:
	float velocity_length = glm::length(camera->velocitie);
	glm::vec3 velocity_normalized = velocity_length != 0 ? glm::normalize(camera->velocitie) : glm::vec3(0.0f);
	glm::vec3 undesired_motion = collInfo.normal * glm::dot(velocity_normalized, collInfo.normal);
	glm::vec3 desired_motion = velocity_normalized - undesired_motion;
	camera->velocitie = desired_motion * velocity_length;

	// Remove penetration (penetration epsilon added to handle infinitely small penetration):
	camera->Position += (collInfo.normal * (collInfo.depth * 0.5f + 0.0001f));
	collider.center = camera->Position;
}

