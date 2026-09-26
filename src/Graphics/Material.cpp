#include "Graphics/Material.h"

#include <cmath>
#include <stdexcept>

namespace FuxEngine
{
    Material::Material(
        Shader& shader,
        Texture& texture,
        Texture* normalMap,
        Texture* emissiveMap,
        float opacity,
        float emissiveStrength,
        float shininess,
        float specularStrength,
        bool doubleSided
    )
        : m_Shader(shader),
        m_Texture(texture),
        m_NormalMap(normalMap),
        m_EmissiveMap(emissiveMap),
        m_Opacity(opacity),
        m_EmissiveStrength(emissiveStrength),
        m_Shininess(shininess),
        m_SpecularStrength(specularStrength),
        m_DoubleSided(doubleSided)
    {
        if (!std::isfinite(m_Opacity) || m_Opacity < 0.0f || m_Opacity > 1.0f)
            throw std::invalid_argument("Material opacity must be between 0 and 1");
        if (!std::isfinite(m_EmissiveStrength) || m_EmissiveStrength < 0.0f)
            throw std::invalid_argument("Material emissive strength cannot be negative");
        if (!std::isfinite(m_Shininess) || m_Shininess <= 0.0f)
            throw std::invalid_argument("Material shininess must be positive");
        if (!std::isfinite(m_SpecularStrength) || m_SpecularStrength < 0.0f || m_SpecularStrength > 1.0f)
            throw std::invalid_argument("Material specular strength must be between 0 and 1");
    }

    void Material::Bind()
    {
        m_Shader.Bind();
        m_Texture.Bind(0);

        m_Shader.SetUniform1i(
            "ourTexture",
            0
        );

        m_Shader.SetUniform1i("hasNormalMap", m_NormalMap ? 1 : 0);
        if (m_NormalMap)
            m_NormalMap->Bind(1);
        m_Shader.SetUniform1i("normalMap", 1);

        m_Shader.SetUniform1i("hasEmissiveMap", m_EmissiveMap ? 1 : 0);
        if (m_EmissiveMap)
            m_EmissiveMap->Bind(2);
        m_Shader.SetUniform1i("emissiveMap", 2);

        m_Shader.SetUniform1f("materialOpacity", m_Opacity);
        m_Shader.SetUniform1f("emissiveStrength", m_EmissiveStrength);
        m_Shader.SetUniform1f("materialShininess", m_Shininess);
        m_Shader.SetUniform1f("materialSpecularStrength", m_SpecularStrength);
        m_Shader.SetUniform1i("doubleSided", m_DoubleSided ? 1 : 0);
    }

    Shader& Material::GetShader() const
    {
        return m_Shader;
    }

    bool Material::IsTransparent() const
    {
        return m_Opacity < 1.0f;
    }

    bool Material::IsDoubleSided() const
    {
        return m_DoubleSided;
    }
}
