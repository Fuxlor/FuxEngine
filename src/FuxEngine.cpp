#include "Core/Window.h"
#include "Core/Camera.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/Light.h"
#include "Core/PointLight.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/Transform.h"
#include "Graphics/Material.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    FuxEngine::Camera* camera =static_cast<FuxEngine::Camera*>(glfwGetWindowUserPointer(window));

    static float lastX = 640.0f;
    static float lastY = 360.0f;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        firstMouse = false;
    }

    float xOffset = static_cast<float>(xpos) - lastX;

    float yOffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera->ProcessMouseMovement(
        xOffset,
        yOffset
    );
}

int main()
{

    float vertices[] = {
        // position              // normal             // color             // UV

        // Front
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

        // Back
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 0.0f,   0.0f, 0.0f,

         // Left
         -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

         // Right
          0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,     1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

          // Top
          -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
           0.5f,  0.5f,  0.5f,     0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
           0.5f,  0.5f, -0.5f,     0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
          -0.5f,  0.5f, -0.5f,     0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

          // Bottom
          -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
           0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
           0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
          -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };
    unsigned int indices[] = {
         0,  1,  2,   2,  3,  0, // Front
         4,  5,  6,   6,  7,  4, // Back
         8,  9, 10,  10, 11,  8, // Left
        12, 13, 14,  14, 15, 12, // Right
        16, 17, 18,  18, 19, 16, // Top
        20, 21, 22,  22, 23, 20  // Bottom
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
            "assets/shaders/basic.vert.glsl",
            "assets/shaders/basic.frag.glsl"
        );

        shader.Bind();

        shader.SetUniform1f(
            "ambientStrength",
            0.1f
        );

        shader.SetUniform1f(
            "constant",
            1.0f
        );

        shader.SetUniform1f(
            "linear",
            0.09f
        );

        shader.SetUniform1f(
            "quadratic",
            0.032f
        );

        shader.SetUniform3f(
            "lightDirection",
            -0.5f,
            -1.0f,
            -0.3f
        );

		// TEXTURE
        FuxEngine::Texture texture("assets/textures/test.jpg");

		// MATERIAL
        FuxEngine::Material material(
            shader,
            texture
        );

        // VAO VBO EBO in mesh
        FuxEngine::Mesh mesh(
            vertices,
            sizeof(vertices),
            indices,
            36
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
        FuxEngine::Camera camera;

        glfwSetWindowUserPointer(window.GetNativeWindow(), &camera);
        glfwSetInputMode(window.GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window.GetNativeWindow(), MouseCallback);

        glEnable(GL_DEPTH_TEST);

		// SCENE + ENTITIES
        FuxEngine::Scene scene;
        FuxEngine::Entity& cube = scene.CreateEntity(mesh, material);
        FuxEngine::Entity& cube2 = scene.CreateEntity(mesh, material);
        scene.CreateLight<PointLight>(
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(1.0f),
            1.0f
        );

        // MAIN LOOP
        float lastFrameTime = 0.0f;
        while (!window.ShouldClose())
        {
			// TIME
            float currentFrameTime = static_cast<float>(glfwGetTime());
            float deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            GLFWwindow* nativeWindow = window.GetNativeWindow();

            // CONTROLS
            if (glfwGetKey(nativeWindow, GLFW_KEY_W) == GLFW_PRESS)
            {
                camera.ProcessKeyboard(
                    FuxEngine::CameraMovement::Forward,
                    deltaTime
                );
            }

            if (glfwGetKey(nativeWindow, GLFW_KEY_S) == GLFW_PRESS)
            {
                camera.ProcessKeyboard(
                    FuxEngine::CameraMovement::Backward,
                    deltaTime
                );
            }

            if (glfwGetKey(nativeWindow, GLFW_KEY_A) == GLFW_PRESS)
            {
                camera.ProcessKeyboard(
                    FuxEngine::CameraMovement::Left,
                    deltaTime
                );
            }

            if (glfwGetKey(nativeWindow, GLFW_KEY_D) == GLFW_PRESS)
            {
                camera.ProcessKeyboard(
                    FuxEngine::CameraMovement::Right,
                    deltaTime
                );
            }

            // EXIT
            if (glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(nativeWindow, true);

			// VIEW MATRIX
            shader.SetUniformMat4(
                "view",
                camera.GetViewMatrix()
            );

			// RENDER
            FuxEngine::Renderer::Clear();

            cube.GetTransform().position.x = -1.5f;
            cube2.GetTransform().position.x = 1.5f;

            cube.GetTransform().rotation.y = glm::degrees(currentFrameTime);
            cube2.GetTransform().rotation.x = glm::degrees(currentFrameTime);

            FuxEngine::Renderer::Draw(scene, camera);

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