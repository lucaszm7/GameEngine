#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <IMGUI/imgui.h>

#include <vector>
#include <array>

#include <mat4.h>
#include <vec4.h>
#include <vec3.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CamMovement 
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NONE
};

enum class ProjectionType
{
	PERSPECTIVE,
	ORTHOGRAPHIC
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
    virtual std::array<float, 13> GetBaseInfo() = 0;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float Near = 0.1f;
    float Far = 5000.0f;

private:
    virtual void updateCameraVectors() = 0;
};

namespace cgl
{
    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera : public BaseCam
    {
    private:
        bool isLookAt = false;
        cgl::vec3 lookAtPos = glm::vec3(0.f, 0.f, 0.f);

    public:

        // camera Attributes
        cgl::vec3 Position;
        cgl::vec3 Front;
        cgl::vec3 Up;
        cgl::vec3 Right;
        cgl::vec3 WorldUp;

        ProjectionType projectionType = ProjectionType::PERSPECTIVE;

        // euler Angles
        float Yaw;
        float Pitch;

        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        float Near = 0.1f;
        float Far = 5000.0f;

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

        std::array<float, 13> GetBaseInfo() override
        {
            return { Position[0], Position[1], Position[2], Up[0], Up[1], Up[2], lookAtPos[0], lookAtPos[1], lookAtPos[2], Zoom, Yaw, Pitch, (float)isLookAt, };
        }

        void OnImGui() override
        {
            if (ImGui::TreeNode("Camera Config"))
            {
                if (ImGui::Button("Reset"))
                    this->Reset();

                ImGui::DragFloat3("Position:", &(Position[0]), 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Yaw:", &Yaw, 0.1f, -glm::pi<float>(), glm::pi<float>());
                ImGui::DragFloat("Pitch:", &Pitch, 0.1f, -glm::pi<float>(), glm::pi<float>());
                ImGui::DragFloat("Near Plane:", &Near, 0.1f, -10000.0f, 10000.0f);
                ImGui::DragFloat("Far Plane:", &Far, 0.1f, -10000.0f, 10000.0f);
                ImGui::DragFloat("Movement Speed:", &MovementSpeed, 0.1f, -1000.0f, 1000.0f);
                ImGui::TreePop();
            }
        }

        void Reset() override
        {
            Front = cgl::vec3(0.0f, 0.0f, -1.0f);
            MovementSpeed = SPEED;
            MouseSensitivity = SENSITIVITY;
            Zoom = ZOOM;

            Far = 5000.0f;
            Near = 0.1f;

            Position = cgl::vec3(0.0f, 0.0f, 0.0f);
            WorldUp = cgl::vec3(0.0f, 1.0f, 0.0f);
            Yaw = YAW;
            Pitch = PITCH;
            updateCameraVectors();
        }

        void SetLookAt(const cgl::vec3& pointInWCS)
        {
            isLookAt = true;
            lookAtPos = (pointInWCS - Position).normalized();
            updateCameraVectors();
        }

        void UnSetLookAt()
        {
            isLookAt = false;
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        inline cgl::mat4 GetViewMatrix() const
        {
            return lookAt(Position, Position + Front, Up);
        }

        cgl::mat4 GetProjectionMatrix(float aspectRatio) const
        {
            if (projectionType == ProjectionType::PERSPECTIVE)
				return getPerspectiveProjection(aspectRatio);
			else
				return getOrthographicProjection(aspectRatio);
        }

        cgl::mat4 getPerspectiveProjection(float aspectRatio) const
        {
            float top = Near * (-glm::tan(Zoom / 2));
            float bottom = -top;
            float right = aspectRatio * top;
            float left = -right;

            float depth = Far - Near;

            cgl::mat4 H = cgl::mat4::identity();
            H[0][2] = (left + right) / (2 * Near);
            H[1][2] = (top + bottom) / (2 * Near);

            cgl::mat4 S = cgl::mat4::identity();
            float yScale = 1.0f / glm::tan(glm::radians(Zoom) / 2);
            float xScale = yScale / aspectRatio;
            S[0][0] = xScale;
            S[1][1] = yScale;

            cgl::mat4 PI = cgl::mat4::identity();
            PI[2][2] = -(Far + Near) / depth;
            PI[2][3] = -(2 * Near * Far) / depth;
            PI[3][2] = -1;
            PI[3][3] = 0;

            cgl::mat4 perspectiveProjection = PI * S * H;

            return perspectiveProjection;
		}

        cgl::mat4 getOrthographicProjection(float aspectRatio) const
        {
            float top = Near * (glm::tan(Zoom / 2));
			float bottom = -top;
			float right = aspectRatio * top;
			float left = -right;

			cgl::mat4 orthographicProjection = cgl::mat4::identity();
			orthographicProjection[0][0] = 2 / (right - left);
			orthographicProjection[1][1] = 2 / (top - bottom);
			orthographicProjection[2][2] = -2 / (Far - Near);
			orthographicProjection[0][3] = -(right + left) / (right - left);
			orthographicProjection[1][3] = -(top + bottom) / (top - bottom);
			orthographicProjection[2][3] = -(Far + Near) / (Far - Near);

			return orthographicProjection;
        }

        // processes input received from any keyboard-like input system. 
        // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(CamMovement direction, double deltaTime) override
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
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override
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
        void ProcessMouseScroll(float yoffset) override
        {
            Zoom -= yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }

        cgl::mat4 lookAt(cgl::vec3 eye, cgl::vec3 dir, cgl::vec3 up) const
        {
            //     right-hand coordinate system
            cgl::vec3 n = (eye - dir).normalized();
            up.normalize();
            cgl::vec3 u = up.cross(n);
            cgl::vec3 v = n.cross(u);

            mat4 cameraBasis;

            cameraBasis[0] = vec4(u).get_array();
            cameraBasis[1] = vec4(v).get_array();
            cameraBasis[2] = vec4(n).get_array();
            cameraBasis.mat[3][3] = 1;

            mat4 cameraTranslation = mat4::identity();

            cameraTranslation.mat[0][3] = -(eye.x);
            cameraTranslation.mat[1][3] = -(eye.y);
            cameraTranslation.mat[2][3] = -(eye.z);

            return (cameraBasis * cameraTranslation);
        }

        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors() override
        {
            // calculate the new Front vector
            cgl::vec3 front;

            // The Front vector is calculated with the multiplication of the Roll, Pitch and Yaw matrix, then simplified them
            // 
            // Roll Matrix
            // | cos(roll) -sin(roll) 0 |
            // | sin(roll)  cos(roll) 0 |
            // | 0          0         1 |
            // 
            // Yaw Matrix
            // | cos(pitch) 0 -sin(pitch) |
            // | 0          1  0          |
            // | sin(pitch) 0  cos(pitch) |
            // 
            // Pitch Matrix
            // | 1         0        0 |
            // | 0 cos(yaw) -sin(yaw) |
            // | 0 sin(yaw)  cos(yaw) |
            // 
            // Resulting Matrix: Roll * Pitch * Yaw
            // | cos(yaw)cos(pitch) -cos(yaw)sin(pitch)sin(roll)-sin(yaw)cos(roll)  -cos(yaw)sin(pitch)cos(roll)+sin(yaw)sin(roll)   |
            // | sin(pitch)          cos(pitch)sin(roll)                               cos(pitch)sin(roll)                           |
            // | sin(yaw)cos(pitch) -sin(yaw)sin(pitch)sin(roll)+cos(yaw)cos(roll)  -sin(yaw)sin(pitch)cos(roll) - cos(yaw)sin(roll) |
            // 
            // Simplified Matrix
            // | cos(yaw)cos(pitch) 0 0 |
            // | sin(pitch)         0 0 |
            // | sin(yaw)cos(pitch) 0 0 |

            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

            Front = isLookAt ? lookAtPos.normalized() : front.normalized();
            // also re-calculate the Right and Up vector
            cgl::vec3 right = Front.cross(WorldUp);
            Right = right.normalized(); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            cgl::vec3 up = Right.cross(Front);
            Up = up.normalized();
            Right = Front.cross(Up).normalized();
        }
    };
}

namespace ogl
{
    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera : public BaseCam
    {
    private:
        bool isLookAt = false;
        glm::vec3 lookAtPos = glm::vec3(0.f, 0.f, 0.f);
    
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
        float Far = 5000.0f;


        // constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
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

        Camera(const cgl::Camera& cglCam)
        {
            Position = glm::vec3(cglCam.Position.x, cglCam.Position.y, cglCam.Position.z);
            Front    = glm::vec3(cglCam.Position.x, cglCam.Position.y, cglCam.Position.z);
            Up       = glm::vec3(cglCam.Position.x, cglCam.Position.y, cglCam.Position.z);
            Right    = glm::vec3(cglCam.Position.x, cglCam.Position.y, cglCam.Position.z);
            WorldUp  = glm::vec3(cglCam.Position.x, cglCam.Position.y, cglCam.Position.z);

            Yaw = cglCam.Yaw;
            Pitch = cglCam.Pitch;

            MovementSpeed = cglCam.MovementSpeed;
            MouseSensitivity = cglCam.MouseSensitivity;
            Zoom = cglCam.Zoom;

            Near = cglCam.Near;
            Far = cglCam.Far;

            updateCameraVectors();
        }

        Camera(std::array<float, 13> param)
            :Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(param[9])
        {
            WorldUp = glm::vec3(0.f, 1.0f, 0.f);
            Position = glm::vec3(param[0], param[1], param[2]);
            Up = glm::vec3(param[3], param[4], param[5]);
            lookAtPos = glm::vec3(param[6], param[7], param[8]);
            Yaw = param[10];
            Pitch = param[11];
            isLookAt = (bool)param[12];
            updateCameraVectors();
        }

        std::array<float, 13> GetBaseInfo() override
        {
            return { Position[0], Position[1], Position[2], Up[0], Up[1], Up[2], lookAtPos[0], lookAtPos[1], lookAtPos[2], Zoom, Yaw, Pitch, (float)isLookAt,};
        }

        void OnImGui() override
        {
            if (ImGui::TreeNode("Camera Config"))
            {
                if (ImGui::Button("Reset"))
                    this->Reset();

                ImGui::DragFloat3("Position:", &Position[0], 0.1f, -10000.0f, 10000.0f);
                ImGui::DragFloat("Yaw:", &Yaw, 0.1f, -2*glm::pi<float>(), 2*glm::pi<float>());
                ImGui::DragFloat("Pitch:", &Pitch, 0.1f, -2*glm::pi<float>(), 2*glm::pi<float>());
                ImGui::DragFloat("Near Plane:", &Near, 0.1f, -10000.0f, 10000.0f);
                ImGui::DragFloat("Far Plane:", &Far, 0.1f, -10000.0f, 10000.0f);
                ImGui::DragFloat("Movement Speed:", &MovementSpeed, 0.1f, -1000.0f, 1000.0f);

                ImGui::TreePop();
            }
        }

        void Reset() override
        {
            Front = glm::vec3(0.0f, 0.0f, -1.0f);
            MovementSpeed = SPEED;
            MouseSensitivity = SENSITIVITY;
            Zoom = ZOOM;

            Far = 5000.0f;
            Near = 0.1f;

            Position = glm::vec3(0.0f, 0.0f, 0.0f);
            WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            Yaw = YAW;
            Pitch = PITCH;
            updateCameraVectors();
        }

        void SetLookAt(const glm::vec3& pointInWCS)
        {
            isLookAt = true;
            lookAtPos = glm::normalize(pointInWCS - Position);
            updateCameraVectors();
        }

        void UnSetLookAt()
        {
            isLookAt = false;
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        inline glm::mat4 GetViewMatrix() const
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        inline glm::mat4 GetProjectionMatrix(float aspectRatio) const
        {
            return glm::perspective(glm::radians(Zoom), aspectRatio, Near, Far);
        }

        // processes input received from any keyboard-like input system. 
        // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(CamMovement direction, double deltaTime) override
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
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override
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
        void ProcessMouseScroll(float yoffset) override
        {
            Zoom -= yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }

        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors() override
        {
            // calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(isLookAt ? lookAtPos : front);
            // also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up = glm::normalize(glm::cross(Right, Front));
        }
    };
}
