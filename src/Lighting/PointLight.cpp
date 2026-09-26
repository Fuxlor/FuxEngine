#include "Lighting/PointLight.h"

namespace FuxEngine
{
    PointLight::PointLight(
        const glm::vec3& position,
        const glm::vec3& color,
        float intensity,
        float constant,
        float linear,
        float quadratic
    )
        : Light(LightType::Point, color, intensity),
        m_Position(position),
        m_Constant(constant),
        m_Linear(linear),
        m_Quadratic(quadratic)
    {}

    const glm::vec3& PointLight::GetPosition() const
    {
        return m_Position;
    }

    float PointLight::GetConstant() const
    {
        return m_Constant;
    }

    float PointLight::GetLinear() const
    {
        return m_Linear;
    }

    float PointLight::GetQuadratic() const
    {
        return m_Quadratic;
    }
}
