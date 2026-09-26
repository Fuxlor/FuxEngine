#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace FuxEngine
{
    class Material
    {
        public:
            Material(
                Shader& shader,
                Texture& texture,
                Texture* normalMap = nullptr,
                Texture* emissiveMap = nullptr,
                float opacity = 1.0f,
                float emissiveStrength = 0.0f,
                float shininess = 32.0f,
                float specularStrength = 0.25f,
                bool doubleSided = false
            );

            void Bind();

            Shader& GetShader() const;
            bool IsTransparent() const;
            bool IsDoubleSided() const;

        private:
            Shader& m_Shader;
            Texture& m_Texture;
            Texture* m_NormalMap;
            Texture* m_EmissiveMap;
            float m_Opacity;
            float m_EmissiveStrength;
            float m_Shininess;
            float m_SpecularStrength;
            bool m_DoubleSided;
    };
}
