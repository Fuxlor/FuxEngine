#include "Core/Light.h"

namespace FuxEngine
{
    Light::Light(
        LightType type,
        const glm::vec3& color,
        float intensity
    )
        : m_Type(type),
        m_Color(color),
        m_Intensity(intensity)
    {}

    LightType Light::GetType() const
    {
        return m_Type;
    }

    const glm::vec3& Light::GetColor() const
    {
        return m_Color;
    }

    float Light::GetIntensity() const
    {
        return m_Intensity;
    }
}