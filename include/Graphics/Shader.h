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
            const std::string& vertexPath,
            const std::string& fragmentPath
        );

        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetUniform1i(const std::string& name, int value) const;
        void SetUniform1f(const std::string& name, float value) const;
        void SetUniform3f(const std::string& name, float x, float y, float z);
        void SetUniformMat4(const std::string& name, const glm::mat4& matrix) const;

        unsigned int GetID() const;

    private:
        static std::string ReadFile(const std::string& path);

        unsigned int m_RendererID = 0;
    };
}