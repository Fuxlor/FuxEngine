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
                const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f)
            );

            void ProcessMouseMovement(
                float xOffset,
                float yOffset
            );

            void ProcessKeyboard(
                CameraMovement direction,
                float deltaTime
            );

            glm::mat4 GetViewMatrix() const;

            const glm::vec3& GetPosition() const;

        private:
            glm::vec3 m_Position;
            glm::vec3 m_Front;
            glm::vec3 m_Up;

            float m_Yaw;
            float m_Pitch;

            float m_MovementSpeed = 2.5f;
            float m_MouseSensitivity = 0.1f;

            void UpdateVectors();
    };
}