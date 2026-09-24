#pragma once

#include <glm/glm.hpp>

namespace FuxEngine
{
    class Transform
    {
        public:
            Transform();

            glm::mat4 GetMatrix() const;

            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;
    };
}