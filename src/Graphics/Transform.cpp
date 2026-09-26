#include "Graphics/Transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace FuxEngine
{
    Transform::Transform()
        : m_Position(0.0f),
        m_Rotation(0.0f),
        m_Scale(1.0f)
    {}

    const glm::vec3& Transform::GetPosition() const
    {
        return m_Position;
    }

    const glm::vec3& Transform::GetRotation() const
    {
        return m_Rotation;
    }

    const glm::vec3& Transform::GetScale() const
    {
        return m_Scale;
    }

    void Transform::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
    }

    void Transform::SetRotation(const glm::vec3& rotationDegrees)
    {
        m_Rotation = rotationDegrees;
    }

    void Transform::SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
    }

    void Transform::Translate(const glm::vec3& offset)
    {
        m_Position += offset;
    }

    void Transform::Rotate(const glm::vec3& rotationDegrees)
    {
        m_Rotation += rotationDegrees;
    }

    void Transform::Scale(const glm::vec3& scaleFactor)
    {
        m_Scale *= scaleFactor;
    }

    glm::mat4 Transform::GetLocalMatrix() const
    {
        glm::mat4 matrix(1.0f);

        matrix = glm::translate(
            matrix,
            m_Position
        );

        matrix = glm::rotate(
            matrix,
            glm::radians(m_Rotation.x),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        matrix = glm::rotate(
            matrix,
            glm::radians(m_Rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        matrix = glm::rotate(
            matrix,
            glm::radians(m_Rotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        matrix = glm::scale(
            matrix,
            m_Scale
        );

        return matrix;
    }

    glm::mat4 Transform::GetWorldMatrix() const
    {
        // Parent transforms are not part of the scene graph yet.
        return GetLocalMatrix();
    }

    glm::mat4 Transform::GetMatrix() const
    {
        return GetWorldMatrix();
    }
}
