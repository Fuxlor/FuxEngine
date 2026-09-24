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