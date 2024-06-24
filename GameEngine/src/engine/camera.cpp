#include "camera.h"

void ogl::Camera::ProcessKeyboard(CamMovement direction, double deltaTime)
{
    float baseVelocity = MovementSpeed * (float)deltaTime;

    if (direction == CamMovement::FORWARD)
        Position += Front * baseVelocity;
    if (direction == CamMovement::BACKWARD)
        Position -= Front * baseVelocity;
    if (direction == CamMovement::LEFT)
        Position -= Right * baseVelocity;
    if (direction == CamMovement::RIGHT)
        Position += Right * baseVelocity;
    if (direction == CamMovement::UP)
        Position += Up * baseVelocity;
    if (direction == CamMovement::DOWN)
        Position -= Up * baseVelocity;
}