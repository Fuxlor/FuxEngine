#include "Graphics/DirectionalLight.h"

namespace FuxEngine
{
    DirectionalLight::DirectionalLight(
        const glm::vec3& direction,
        const glm::vec3& color,
        float intensity
    )
        : Light(
            LightType::Directional,
            color,
            intensity
        ),
        m_Direction(direction)
    {}

    const glm::vec3& DirectionalLight::GetDirection() const
    {
        return m_Direction;
    }
}