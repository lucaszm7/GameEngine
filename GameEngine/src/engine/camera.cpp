#include "camera.h"

void ogl::Camera::ProcessKeyboard(CamMovement direction, double deltaTime)
{
    float baseVelocity = MovementSpeed * (float)deltaTime;
    velocitie -= velocitie * 3.0f * (float)deltaTime;

    if (direction == CamMovement::FORWARD)
        velocitie += Front * baseVelocity;
    if (direction == CamMovement::BACKWARD)
        velocitie -= Front * baseVelocity;
    if (direction == CamMovement::LEFT)
        velocitie -= Right * baseVelocity;
    if (direction == CamMovement::RIGHT)
        velocitie += Right * baseVelocity;
    if (direction == CamMovement::UP)
        velocitie += Up * baseVelocity;
    if (direction == CamMovement::DOWN)
        velocitie -= Up * baseVelocity;

    Position += velocitie;
}