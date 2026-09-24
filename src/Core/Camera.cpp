#include "Core/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace FuxEngine
{
    Camera::Camera(const glm::vec3& position)
        : m_Position(position),
        m_Front(0.0f, 0.0f, -1.0f),
        m_Up(0.0f, 1.0f, 0.0f),
        m_Yaw(-90.0f),
        m_Pitch(0.0f)
    {
        UpdateVectors();
    }

    void Camera::ProcessMouseMovement(
        float xOffset,
        float yOffset
    )
    {
        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;

        m_Yaw += xOffset;
        m_Pitch += yOffset;

        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;

        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;

        UpdateVectors();
    }

    void Camera::ProcessKeyboard(
        CameraMovement direction,
        float deltaTime
    )
    {
        float velocity = m_MovementSpeed * deltaTime;

        glm::vec3 right = glm::normalize(
            glm::cross(m_Front, m_Up)
        );

        switch (direction)
        {
            case CameraMovement::Forward:
                m_Position += m_Front * velocity;
                break;

            case CameraMovement::Backward:
                m_Position -= m_Front * velocity;
                break;

            case CameraMovement::Left:
                m_Position -= right * velocity;
                break;

            case CameraMovement::Right:
                m_Position += right * velocity;
                break;
        }
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(
            m_Position,
            m_Position + m_Front,
            m_Up
        );
    }

    const glm::vec3& Camera::GetPosition() const
    {
        return m_Position;
    }

    void Camera::UpdateVectors()
    {
        glm::vec3 direction;

        direction.x =
            cos(glm::radians(m_Yaw)) *
            cos(glm::radians(m_Pitch));

        direction.y =
            sin(glm::radians(m_Pitch));

        direction.z =
            sin(glm::radians(m_Yaw)) *
            cos(glm::radians(m_Pitch));

        m_Front = glm::normalize(direction);
    }
}