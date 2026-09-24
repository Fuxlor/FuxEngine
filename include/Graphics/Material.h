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
                Texture& texture
            );

            void Bind();

            Shader& GetShader();

        private:
            Shader& m_Shader;
            Texture& m_Texture;
    };
}