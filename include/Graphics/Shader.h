#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
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
        void SetUniformMat4(const std::string& name, const glm::mat4& matrix) const;

        unsigned int GetID() const;

    private:
        unsigned int m_RendererID = 0;
    };
}