#pragma once

#include <glm/glm.hpp>

namespace FuxEngine
{
    class Transform
    {
        public:
            Transform();

            const glm::vec3& GetPosition() const;
            const glm::vec3& GetRotation() const;
            const glm::vec3& GetScale() const;

            void SetPosition(const glm::vec3& position);
            void SetRotation(const glm::vec3& rotationDegrees);
            void SetScale(const glm::vec3& scale);

            void Translate(const glm::vec3& offset);
            void Rotate(const glm::vec3& rotationDegrees);
            void Scale(const glm::vec3& scaleFactor);

            // Local transform matrix. With no parent, this is also the world matrix.
            glm::mat4 GetLocalMatrix() const;
            glm::mat4 GetWorldMatrix() const;
            glm::mat4 GetMatrix() const;

        private:
            glm::vec3 m_Position;
            glm::vec3 m_Rotation;
            glm::vec3 m_Scale;
    };
}
