#pragma once

#include "Lighting/Light.h"

namespace FuxEngine
{
    class DirectionalLight : public Light
    {
    public:
        DirectionalLight(
            const glm::vec3& direction,
            const glm::vec3& color,
            float intensity
        );

        const glm::vec3& GetDirection() const;

    private:
        glm::vec3 m_Direction;
    };
}
