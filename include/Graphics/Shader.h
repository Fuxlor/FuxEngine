#pragma once

#include <glad/glad.h>
#include <string>

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

        void SetUniform1i(const std::string& name, int value) const;

        unsigned int GetID() const;

    private:
        unsigned int m_ID = 0;
    };
}