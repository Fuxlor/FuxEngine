#include "Graphics/SpotLight.h"

namespace FuxEngine
{
    SpotLight::SpotLight(
        const glm::vec3& position,
        const glm::vec3& direction,
        const glm::vec3& color,
        float intensity,
        float innerCutoff,
        float outerCutoff,
        float constant,
        float linear,
        float quadratic
    )
        : Light(
            LightType::Spot,
            color,
            intensity
        ),
        m_Position(position),
        m_Direction(direction),
        m_InnerCutoff(innerCutoff),
        m_OuterCutoff(outerCutoff),
        m_Constant(constant),
        m_Linear(linear),
        m_Quadratic(quadratic)
    {}

    const glm::vec3& SpotLight::GetPosition() const
    {
        return m_Position;
    }

    const glm::vec3& SpotLight::GetDirection() const
    {
        return m_Direction;
    }

    float SpotLight::GetInnerCutoff() const
    {
        return m_InnerCutoff;
    }

    float SpotLight::GetOuterCutoff() const
    {
        return m_OuterCutoff;
    }

    float SpotLight::GetConstant() const
    {
        return m_Constant;
    }

    float SpotLight::GetLinear() const
    {
        return m_Linear;
    }

    float SpotLight::GetQuadratic() const
    {
        return m_Quadratic;
    }
}