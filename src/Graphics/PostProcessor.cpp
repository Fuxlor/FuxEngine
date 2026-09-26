#include "Graphics/PostProcessor.h"

#include <array>
#include <cmath>
#include <stdexcept>

namespace FuxEngine
{
    PostProcessor::PostProcessor(Shader& shader, GLFWwindow* window)
        : m_Shader(shader),
        m_Window(window)
    {
        if (!m_Window)
            throw std::invalid_argument("PostProcessor requires a valid GLFW window");

        ResizeToWindow();

        constexpr std::array<float, 24> quadVertices = {
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f
        };

        glGenVertexArrays(1, &m_QuadVertexArray);
        glGenBuffers(1, &m_QuadVertexBuffer);
        glBindVertexArray(m_QuadVertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            4 * sizeof(float),
            reinterpret_cast<void*>(2 * sizeof(float))
        );
        glBindVertexArray(0);
    }

    PostProcessor::~PostProcessor()
    {
        if (m_QuadVertexBuffer)
            glDeleteBuffers(1, &m_QuadVertexBuffer);
        if (m_QuadVertexArray)
            glDeleteVertexArrays(1, &m_QuadVertexArray);
    }

    void PostProcessor::BeginScene()
    {
        ResizeToWindow();
        m_Framebuffer->Bind();
        glViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void PostProcessor::Present()
    {
        Framebuffer::Unbind();
        glViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        m_Shader.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorAttachment());
        m_Shader.SetUniform1i("hdrScene", 0);
        m_Shader.SetUniform1f("exposure", m_Exposure);
        glBindVertexArray(m_QuadVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void PostProcessor::SetExposure(float exposure)
    {
        if (!std::isfinite(exposure) || exposure < 0.0f)
            throw std::invalid_argument("Post-process exposure cannot be negative");
        m_Exposure = exposure;
    }

    void PostProcessor::ResizeToWindow()
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        if (width <= 0 || height <= 0)
            return;

        if (!m_Framebuffer)
            m_Framebuffer = std::make_unique<Framebuffer>(width, height);
        else
            m_Framebuffer->Resize(width, height);
    }
}
