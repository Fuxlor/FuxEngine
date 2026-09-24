#include "Core/Window.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"

#include <iostream>

int main()
{
    const char* vertexShaderSource = R"(
        #version 330 core

        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec2 aTexCoord;

        out vec3 ourColor;
        out vec2 TexCoord;

        void main()
        {
            gl_Position = vec4(aPos, 0.0, 1.0);
            ourColor = aColor;
            TexCoord = aTexCoord;
        }
    )";
    const char* fragmentShaderSource = R"(
        #version 330 core

        in vec3 ourColor;
        in vec2 TexCoord;

        out vec4 FragColor;

        uniform sampler2D ourTexture;

        void main()
        {
            FragColor = texture(ourTexture, TexCoord);
        }
    )";

    float vertices[] = {
        // position       // couleur
        -0.5f,  0.5f,     1.0f, 0.0f, 0.0f, // 0
         0.5f,  0.5f,     0.0f, 1.0f, 0.0f, // 1
         0.5f, -0.5f,     0.0f, 0.0f, 1.0f, // 2
        -0.5f, -0.5f,     1.0f, 1.0f, 0.0f  // 3
    };
    unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3
    };

    try
    {
        // WINDOW
        FuxEngine::Window window(
            1280, 
            720, 
            "FuxEngine"
        );

        std::cout << glGetString(GL_VENDOR) << '\n';
        std::cout << glGetString(GL_RENDERER) << '\n';
        std::cout  << glGetString(GL_VERSION) << '\n';

        FuxEngine::Renderer::SetClearColor(
            0.1f,
            0.1f,
            0.15f,
            1.0f
        );

        // SHADERS
        FuxEngine::Shader shader(
            vertexShaderSource, 
            fragmentShaderSource
        );

        shader.Bind();

        // VAO VBO EBO in mesh
        FuxEngine::Mesh mesh(
            vertices,
            sizeof(vertices),
            indices,
            6
        );

        // MAIN LOOP
        while (!window.ShouldClose())
        {
            FuxEngine::Renderer::Clear();

            shader.Bind();

            FuxEngine::Renderer::Draw(mesh);

            window.Update();
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "FuxEngine error: " << exception.what() << '\n';
        return -1;
    }

    return 0;
}