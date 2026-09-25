#pragma once

#include <glm/glm.hpp>

namespace FuxEngine
{
    enum class LightType
    {
        Point,
        Directional,
        Spot
    };

    class Light
    {
        public:
            Light(
                LightType type,
                const glm::vec3& color,
                float intensity
            );

            virtual ~Light() = default;

            LightType GetType() const;

            const glm::vec3& GetColor() const;
            float GetIntensity() const;

        protected:
            LightType m_Type;
            glm::vec3 m_Color;
            float m_Intensity;
    };
}