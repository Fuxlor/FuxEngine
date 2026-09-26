#include "Core/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <stdexcept>

namespace FuxEngine
{
    Camera::Camera(
        const glm::vec3& position,
        float fieldOfView,
        float aspectRatio,
        float nearPlane,
        float farPlane
    )
        : m_Position(position),
        m_Front(0.0f, 0.0f, -1.0f),
        m_Up(0.0f, 1.0f, 0.0f),
        m_Yaw(-90.0f),
        m_Pitch(0.0f),
        m_FieldOfView(fieldOfView),
        m_AspectRatio(aspectRatio),
        m_NearPlane(nearPlane),
        m_FarPlane(farPlane)
    {
        SetPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);
        UpdateVectors();
    }

    void Camera::SetPerspective(
        float fieldOfView,
        float aspectRatio,
        float nearPlane,
        float farPlane
    )
    {
        if (!std::isfinite(fieldOfView) || fieldOfView <= 0.0f || fieldOfView >= 180.0f)
            throw std::invalid_argument("Camera FOV must be between 0 and 180 degrees");
        if (!std::isfinite(aspectRatio) || aspectRatio <= 0.0f)
            throw std::invalid_argument("Camera aspect ratio must be positive");
        if (!std::isfinite(nearPlane) || !std::isfinite(farPlane) || nearPlane <= 0.0f || farPlane <= nearPlane)
            throw std::invalid_argument("Camera clipping planes must satisfy 0 < near < far");

        m_FieldOfView = fieldOfView;
        m_AspectRatio = aspectRatio;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
    }

    void Camera::SetAspectRatio(float aspectRatio)
    {
        if (!std::isfinite(aspectRatio) || aspectRatio <= 0.0f)
            throw std::invalid_argument("Camera aspect ratio must be positive");
        m_AspectRatio = aspectRatio;
    }

    void Camera::SetMovementSpeed(float speed)
    {
        if (!std::isfinite(speed) || speed < 0.0f)
            throw std::invalid_argument("Camera movement speed cannot be negative");
        m_MovementSpeed = speed;
    }

    void Camera::SetMouseSensitivity(float sensitivity)
    {
        if (!std::isfinite(sensitivity) || sensitivity < 0.0f)
            throw std::invalid_argument("Camera mouse sensitivity cannot be negative");
        m_MouseSensitivity = sensitivity;
    }

    void Camera::SetYawPitch(float yaw, float pitch)
    {
        if (!std::isfinite(yaw) || !std::isfinite(pitch))
            throw std::invalid_argument("Camera yaw and pitch must be finite");
        m_Yaw = yaw;
        m_Pitch = glm::clamp(pitch, -89.0f, 89.0f);
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

    glm::mat4 Camera::GetProjectionMatrix() const
    {
        return glm::perspective(
            glm::radians(m_FieldOfView),
            m_AspectRatio,
            m_NearPlane,
            m_FarPlane
        );
    }

    const glm::vec3& Camera::GetPosition() const
    {
        return m_Position;
    }

    float Camera::GetFieldOfView() const { return m_FieldOfView; }
    float Camera::GetAspectRatio() const { return m_AspectRatio; }
    float Camera::GetNearPlane() const { return m_NearPlane; }
    float Camera::GetFarPlane() const { return m_FarPlane; }
    float Camera::GetMovementSpeed() const { return m_MovementSpeed; }
    float Camera::GetMouseSensitivity() const { return m_MouseSensitivity; }
    float Camera::GetYaw() const { return m_Yaw; }
    float Camera::GetPitch() const { return m_Pitch; }

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
