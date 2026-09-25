#pragma once

#include "Core/Light.h"

namespace FuxEngine
{
    class SpotLight : public Light
    {
    public:
        SpotLight(
            const glm::vec3& position,
            const glm::vec3& direction,
            const glm::vec3& color,
            float intensity,
            float innerCutoff,
            float outerCutoff,
            float constant,
            float linear,
            float quadratic
        );

        const glm::vec3& GetPosition() const;
        const glm::vec3& GetDirection() const;

        float GetInnerCutoff() const;
        float GetOuterCutoff() const;

        float GetConstant() const;
        float GetLinear() const;
        float GetQuadratic() const;

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Direction;

        float m_InnerCutoff;
        float m_OuterCutoff;

        float m_Constant;
        float m_Linear;
        float m_Quadratic;
    };
}