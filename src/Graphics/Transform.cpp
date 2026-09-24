#include "Graphics/Transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace FuxEngine
{
    Transform::Transform()
        : position(0.0f),
        rotation(0.0f),
        scale(1.0f)
    {}

    glm::mat4 Transform::GetMatrix() const
    {
        glm::mat4 matrix(1.0f);

        matrix = glm::translate(
            matrix,
            position
        );

        matrix = glm::rotate(
            matrix,
            glm::radians(rotation.x),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        matrix = glm::rotate(
            matrix,
            glm::radians(rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        matrix = glm::rotate(
            matrix,
            glm::radians(rotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        matrix = glm::scale(
            matrix,
            scale
        );

        return matrix;
    }
}