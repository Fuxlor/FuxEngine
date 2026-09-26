#include "Core/Window.h"

#include <iostream>
#include <stdexcept>

namespace FuxEngine
{
    Window::Window(int width, int height, const char* title)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        m_Window = glfwCreateWindow(
            width, 
            height, 
            title, 
            nullptr,  
            nullptr
        );

        if (!m_Window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader(
            reinterpret_cast<GLADloadproc>(glfwGetProcAddress)
        ))
        {
            glfwDestroyWindow(m_Window);
            glfwTerminate();

            throw std::runtime_error("Failed to initialize GLAD");
        }

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(
            m_Window,
            [](GLFWwindow*, int framebufferWidth, int framebufferHeight)
            {
                glViewport(0, 0, framebufferWidth, framebufferHeight);
            }
        );
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    void Window::Update()
    {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    GLFWwindow* Window::GetNativeWindow() const
    {
        return m_Window;
    }
}
