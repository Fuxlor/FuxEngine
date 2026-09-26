#pragma once

#include <glm/glm.hpp>

namespace FuxEngine
{
    enum class CameraMovement
    {
        Forward,
        Backward,
        Left,
        Right
    };

    class Camera
    {
        public:
            Camera(
                const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f),
                float fieldOfView = 45.0f,
                float aspectRatio = 16.0f / 9.0f,
                float nearPlane = 0.1f,
                float farPlane = 100.0f
            );

            void ProcessMouseMovement(
                float xOffset,
                float yOffset
            );

            void ProcessKeyboard(
                CameraMovement direction,
                float deltaTime
            );

            void SetPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
            void SetAspectRatio(float aspectRatio);
            void SetMovementSpeed(float speed);
            void SetMouseSensitivity(float sensitivity);
            void SetYawPitch(float yaw, float pitch);

            glm::mat4 GetViewMatrix() const;
            glm::mat4 GetProjectionMatrix() const;

            const glm::vec3& GetPosition() const;
            float GetFieldOfView() const;
            float GetAspectRatio() const;
            float GetNearPlane() const;
            float GetFarPlane() const;
            float GetMovementSpeed() const;
            float GetMouseSensitivity() const;
            float GetYaw() const;
            float GetPitch() const;

        private:
            glm::vec3 m_Position;
            glm::vec3 m_Front;
            glm::vec3 m_Up;

            float m_Yaw;
            float m_Pitch;

            float m_FieldOfView;
            float m_AspectRatio;
            float m_NearPlane;
            float m_FarPlane;
            float m_MovementSpeed = 2.5f;
            float m_MouseSensitivity = 0.1f;

            void UpdateVectors();
    };
}
