#pragma once

#include <glad/glad.h>

namespace FuxEngine
{
    class Shader
    {
    public:
        Shader(
            const char* vertexSource,
            const char* fragmentSource
        );

        ~Shader();

        void Bind() const;
        void Unbind() const;

        unsigned int GetID() const;

    private:
        unsigned int m_ID = 0;
    };
}