#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace FuxEngine
{
    class Window
    {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        bool ShouldClose() const;
        void Update();

        GLFWwindow* GetNativeWindow() const;

    private:
        GLFWwindow* m_Window = nullptr;
    };
}