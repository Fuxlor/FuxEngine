#pragma once

#include "Graphics/Framebuffer.h"
#include "Graphics/Shader.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace FuxEngine
{
    class PostProcessor
    {
    public:
        PostProcessor(Shader& shader, GLFWwindow* window);
        ~PostProcessor();

        PostProcessor(const PostProcessor&) = delete;
        PostProcessor& operator=(const PostProcessor&) = delete;

        void BeginScene();
        void Present();
        void SetExposure(float exposure);

    private:
        void ResizeToWindow();

        Shader& m_Shader;
        GLFWwindow* m_Window;
        std::unique_ptr<Framebuffer> m_Framebuffer;
        unsigned int m_QuadVertexArray = 0;
        unsigned int m_QuadVertexBuffer = 0;
        float m_Exposure = 1.0f;
    };
}
