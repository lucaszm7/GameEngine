#include "camera.h"

void ogl::Camera::ProcessKeyboard(CamMovement direction, double deltaTime)
{
    float baseVelocity = MovementSpeed * (float)deltaTime;
    velocity -= velocity * 3.0f * (float)deltaTime;

    if (direction == CamMovement::FORWARD)
        velocity += Front * baseVelocity;
    if (direction == CamMovement::BACKWARD)
        velocity -= Front * baseVelocity;
    if (direction == CamMovement::LEFT)
        velocity -= Right * baseVelocity;
    if (direction == CamMovement::RIGHT)
        velocity += Right * baseVelocity;
    if (direction == CamMovement::UP)
        velocity += Up * baseVelocity;
    if (direction == CamMovement::DOWN)
        velocity -= Up * baseVelocity;

    Position += velocity;
}