#include "Graphics/Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>

namespace FuxEngine
{
    Shader::Shader(
        const char* vertexSource,
        const char* fragmentSource
    )
    {
        int success;
        char infoLog[512];
    
        // =========================
        // Vertex Shader
        // =========================
        
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(
            vertexShader, 
            1, 
            &vertexSource, 
            nullptr
        );
        glCompileShader(vertexShader);

        glGetShaderiv(
            vertexShader, 
            GL_COMPILE_STATUS, 
            &success
        );
        if (!success)
        {
            glGetShaderInfoLog(
                vertexShader, 
                512, 
                nullptr, 
                infoLog
            );

            std::cerr << "Vertex shader compilation failed:\n" << infoLog << '\n';

            glDeleteShader(vertexShader);

            throw std::runtime_error(
                "Vertex shader compilation failed"
            );
        }

        // =========================
        // Fragment Shader
        // =========================
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(
            fragmentShader, 
            1, 
            &fragmentSource, 
            nullptr
        );
        glCompileShader(fragmentShader);

        glGetShaderiv(
            fragmentShader, 
            GL_COMPILE_STATUS, 
            &success
        );
        if (!success)
        {
            glGetShaderInfoLog(
                fragmentShader, 
                512, 
                nullptr, 
                infoLog
            );

            std::cerr << "Fragment shader compilation failed:\n" << infoLog << '\n';

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            throw std::runtime_error(
                "Fragment shader compilation failed"
            );
        }

        // =========================
        // Shader Program
        // =========================

        m_RendererID = glCreateProgram();

        glAttachShader(
            m_RendererID, 
            vertexShader
        );
        glAttachShader(
            m_RendererID, 
            fragmentShader
        );
        glLinkProgram(m_RendererID);

        glGetProgramiv(
            m_RendererID, 
            GL_LINK_STATUS, 
            &success
        );
        if (!success)
        {
            glGetProgramInfoLog(
                m_RendererID, 
                512, 
                nullptr, 
                infoLog
            );

            std::cerr << "Shader program linking failed:\n" << infoLog << '\n';

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(m_RendererID);

            m_RendererID = 0;

            throw std::runtime_error(
                "Shader program linking failed"
            );
        }

        // Les shaders ne sont plus nécessaires après le linking
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    void Shader::SetUniform1i(const std::string& name, int value) const
    {
        glUniform1i(
            glGetUniformLocation(m_RendererID, name.c_str()),
            value
        );
    }

    void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) const
    {
        glUniformMatrix4fv(
            glGetUniformLocation(m_RendererID, name.c_str()),
            1,
            GL_FALSE,
            glm::value_ptr(matrix)
        );
    }

    unsigned int Shader::GetID() const
    {
        return m_RendererID;
    }

    Shader::~Shader()
    {
        if (m_RendererID != 0)
        {
            glDeleteProgram(m_RendererID);
        }
    }
}