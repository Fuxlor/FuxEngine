#pragma once

#include "Lighting/Light.h"

namespace FuxEngine
{
    class PointLight : public Light
    {
        public:
            PointLight(
                const glm::vec3& position,
                const glm::vec3& color,
                float intensity,
                float constant = 1.0f,
                float linear = 0.09f,
                float quadratic = 0.032f
            );

            const glm::vec3& GetPosition() const;

            float GetConstant() const;
            float GetLinear() const;
            float GetQuadratic() const;

        private:
            glm::vec3 m_Position;

            float m_Constant;
            float m_Linear;
            float m_Quadratic;
    };
}
