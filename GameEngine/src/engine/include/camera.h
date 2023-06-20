#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include <vector>
#include "math/vec3.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CamMovement 
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
static const float YAW = -90.0f;
static const float PITCH = 0.0f;
static const float SPEED = 10.0f;
static const float SENSITIVITY = 0.1f;
static const float ZOOM = 45.0f;

class BaseCam
{
public:
    virtual void OnImGui() = 0;
    virtual void Reset() = 0;
    virtual void ProcessKeyboard(CamMovement direction, double deltaTime) = 0;
    virtual void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) = 0;
    virtual void ProcessMouseScroll(float yoffset) = 0;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float Near = 0.1f;
    float Far = 1000.0f;

private:
    virtual void updateCameraVectors() = 0;
};

namespace cgl
{
    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera : public BaseCam
    {
    public:

        // camera Attributes
        cgl::vec3 Position;
        cgl::vec3 Front;
        cgl::vec3 Up;
        cgl::vec3 Right;
        cgl::vec3 WorldUp;

        // euler Angles
        float Yaw;
        float Pitch;

        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        float Near = 0.1f;
        float Far = 1000.0f;

        // constructor with vectors
        Camera(cgl::vec3 position = cgl::vec3(0.0f, 0.0f, 0.0f), cgl::vec3 up = cgl::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
            : Front(cgl::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
            : Front(cgl::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
            Position = cgl::vec3(posX, posY, posZ);
            WorldUp = cgl::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        void OnImGui()
        {
            if (ImGui::TreeNode("Camera Config"))
            {
                if (ImGui::Button("Reset"))
                    this->Reset();

                ImGui::DragFloat3("Position:", &(std::static_cast<float>(Position[0])), 0.1f, -100.0f, 100.0f);
                ImGui::DragFloat("Yaw:", &Yaw, 0.1f, -glm::pi<float>(), glm::pi<float>());
                ImGui::DragFloat("Pitch:", &Pitch, 0.1f, -glm::pi<float>(), glm::pi<float>());
                ImGui::DragFloat("Near Plane:", &Near, 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Far Plane:", &Far, 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Movement Speed:", &MovementSpeed, 0.1f, -1000.0f, 1000.0f);
                ImGui::TreePop();
            }
        }

        void Reset()
        {
            Front = cgl::vec3(0.0f, 0.0f, -1.0f);
            MovementSpeed = SPEED;
            MouseSensitivity = SENSITIVITY;
            Zoom = ZOOM;

            Far = 1000.0f;
            Near = 0.1f;

            Position = cgl::vec3(0.0f, 0.0f, 0.0f);
            WorldUp = cgl::vec3(0.0f, 1.0f, 0.0f);
            Yaw = YAW;
            Pitch = PITCH;
            updateCameraVectors();
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        cgl::mat4 GetViewMatrix(cgl::vec3* lookAt = nullptr) const
        {
            if (!lookAt)
                return cgl::lookAt(Position, Position + Front, Up);
            else
                return cgl::lookAt(Position, *lookAt, Up);
        }

        cgl::mat4 GetProjectionMatrix(float aspectRatio) const
        {
            return cgl::perspective(cgl::radians(Zoom), aspectRatio, Near, Far);
        }

        // processes input received from any keyboard-like input system. 
        // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(CamMovement direction, double deltaTime)
        {
            double velocity = MovementSpeed * deltaTime;
            if (direction == CamMovement::FORWARD)
                Position += Front * velocity;
            if (direction == CamMovement::BACKWARD)
                Position -= Front * velocity;
            if (direction == CamMovement::LEFT)
                Position -= Right * velocity;
            if (direction == CamMovement::RIGHT)
                Position += Right * velocity;
            if (direction == CamMovement::UP)
                Position += Up * velocity;
            if (direction == CamMovement::DOWN)
                Position -= Up * velocity;
        }

        // processes input received from a mouse input system. 
        // Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw = glm::mod(Yaw + xoffset, 360.0f);
            Pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {
            Zoom -= yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }

    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {
            // calculate the new Front vector
            cgl::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = front.normalized();
            // also re-calculate the Right and Up vector
            Right = Front.cross(WorldUp).normalized(); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up = Right.cross(Front).normalized();
        }
    };
}

namespace ogl
{
    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera : public BaseCam
    {
    public:

        // camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        // euler Angles
        float Yaw;
        float Pitch;

        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        float Near = 0.1f;
        float Far = 1000.0f;

        // constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
            : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
            : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        void OnImGui()
        {
            if (ImGui::TreeNode("Camera Config"))
            {
                if (ImGui::Button("Reset"))
                    this->Reset();

                ImGui::DragFloat3("Position:", &Position[0], 0.1f, -100.0f, 100.0f);
                ImGui::DragFloat("Yaw:", &Yaw, 0.1f, -glm::pi<float>(), glm::pi<float>());
                ImGui::DragFloat("Pitch:", &Pitch, 0.1f, -glm::pi<float>(), glm::pi<float>());
                ImGui::DragFloat("Near Plane:", &Near, 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Far Plane:", &Far, 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Movement Speed:", &MovementSpeed, 0.1f, -1000.0f, 1000.0f);
                ImGui::TreePop();
            }
        }

        void Reset()
        {
            Front = glm::vec3(0.0f, 0.0f, -1.0f);
            MovementSpeed = SPEED;
            MouseSensitivity = SENSITIVITY;
            Zoom = ZOOM;

            Far = 1000.0f;
            Near = 0.1f;

            Position = glm::vec3(0.0f, 0.0f, 0.0f);
            WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            Yaw = YAW;
            Pitch = PITCH;
            updateCameraVectors();
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix(glm::vec3* lookAt = nullptr) const
        {
            if (!lookAt)
                return glm::lookAt(Position, Position + Front, Up);
            else
                return glm::lookAt(Position, *lookAt, Up);
        }

        glm::mat4 GetProjectionMatrix(float aspectRatio) const
        {
            return glm::perspective(glm::radians(Zoom), aspectRatio, Near, Far);
        }

        // processes input received from any keyboard-like input system. 
        // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(CamMovement direction, double deltaTime)
        {
            float velocity = MovementSpeed * (float)deltaTime;
            if (direction == CamMovement::FORWARD)
                Position += Front * velocity;
            if (direction == CamMovement::BACKWARD)
                Position -= Front * velocity;
            if (direction == CamMovement::LEFT)
                Position -= Right * velocity;
            if (direction == CamMovement::RIGHT)
                Position += Right * velocity;
            if (direction == CamMovement::UP)
                Position += Up * velocity;
            if (direction == CamMovement::DOWN)
                Position -= Up * velocity;
        }

        // processes input received from a mouse input system. 
        // Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw = glm::mod(Yaw + xoffset, 360.0f);
            Pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {
            Zoom -= yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }

    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {
            // calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            // also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up = glm::normalize(glm::cross(Right, Front));
        }
    };
}
