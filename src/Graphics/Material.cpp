#include "Graphics/Material.h"

namespace FuxEngine
{
    Material::Material(
        Shader& shader,
        Texture& texture
    )
        : m_Shader(shader),
        m_Texture(texture)
    {}

    void Material::Bind()
    {
        m_Shader.Bind();
        m_Texture.Bind(0);

        m_Shader.SetUniform1i(
            "ourTexture",
            0
        );
    }

    Shader& Material::GetShader()
    {
        return m_Shader;
    }
}