#include "Core/Window.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

struct CameraState
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;

    float lastX = 640.0f;
    float lastY = 360.0f;
    bool firstMouse = true;
};

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    CameraState* camera = static_cast<CameraState*>(
            glfwGetWindowUserPointer(window)
        );

    if (camera->firstMouse)
    {
        camera->lastX = static_cast<float>(xpos);
        camera->lastY = static_cast<float>(ypos);
        camera->firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - camera->lastX;
    float yoffset = camera->lastY - static_cast<float>(ypos);

    camera->lastX = static_cast<float>(xpos);
    camera->lastY = static_cast<float>(ypos);

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->yaw += xoffset;
    camera->pitch += yoffset;

    if (camera->pitch > 89.0f)  camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    direction.y = sin(glm::radians(camera->pitch));
    direction.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));

    camera->front = glm::normalize(direction);
}

int main()
{
    const char* vertexShaderSource = R"(
        #version 330 core

        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec2 aTexCoord;

        out vec3 ourColor;
        out vec2 TexCoord;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
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
        // position       // couleur          // UV
        -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // 0
         0.5f,  0.5f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // 1
         0.5f, -0.5f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // 2
        -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // 3
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

		// TEXTURE
        FuxEngine::Texture texture("assets/textures/test.jpg");

        shader.SetUniform1i("ourTexture", 0);

        // VAO VBO EBO in mesh
        FuxEngine::Mesh mesh(
            vertices,
            sizeof(vertices),
            indices,
            6
        );

		// PROJECTION MATRIX
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),   // field of view (FOV)
            1280.0f / 720.0f,      // aspect ratio
            0.1f,                  // near plane
            100.0f                 // far plane
        );
        shader.SetUniformMat4("projection", projection);

		// CAMERA
        CameraState camera;

        glfwSetWindowUserPointer(window.GetNativeWindow(), &camera);
        glfwSetInputMode(window.GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window.GetNativeWindow(), MouseCallback);

        float lastFrameTime = 0.0f;


        // MAIN LOOP
        while (!window.ShouldClose())
        {
            float currentFrameTime = static_cast<float>(glfwGetTime());
            float deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            float cameraSpeed = 2.5f * deltaTime;

            GLFWwindow* nativeWindow = window.GetNativeWindow();

            // CONTROLS
            if (glfwGetKey(nativeWindow, GLFW_KEY_W) == GLFW_PRESS)
                camera.position += cameraSpeed * camera.front;

            if (glfwGetKey(nativeWindow, GLFW_KEY_S) == GLFW_PRESS)
                camera.position -= cameraSpeed * camera.front;

            if (glfwGetKey(nativeWindow, GLFW_KEY_A) == GLFW_PRESS)
                camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;

            if (glfwGetKey(nativeWindow, GLFW_KEY_D) == GLFW_PRESS)
                camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;

            // EXIT
            if (glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(nativeWindow, true);

			// VIEW MATRIX
            glm::mat4 view = glm::lookAt(
                camera.position,
                camera.position + camera.front,
                camera.up
            );

            shader.SetUniformMat4("view", view);

			// RENDER
            FuxEngine::Renderer::Clear();

            shader.Bind();
            texture.Bind(0);

            float time = static_cast<float>(glfwGetTime());

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, time, glm::vec3(0.0f, 0.0f, 1.0f));

            shader.SetUniformMat4("model", model);

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