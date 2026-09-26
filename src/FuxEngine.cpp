#include "Core/Window.h"
#include "Core/Camera.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Lighting/Light.h"
#include "Lighting/PointLight.h"
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

        FuxEngine::Renderer::SetAmbientStrength(0.1f);

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
        FuxEngine::Renderer::SetProjection(projection);

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
        cube.GetTransform().SetPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
        cube2.GetTransform().SetPosition(glm::vec3(1.5f, 0.0f, 0.0f));
        scene.CreateLight<FuxEngine::PointLight>(
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(1.0f),
            1.0f,
            1.0f,
			0.09f,
			0.032f
        );
        scene.CreateLight<FuxEngine::SpotLight>(
            glm::vec3(-2.0f, -2.0f, 0.0f), // position sous les cubes
            glm::vec3(0.0f, 1.0f, 0.0f),  // direction vers le haut
            glm::vec3(1.0f, 0.85f, 0.65f), // couleur chaude
            3.0f,                           // intensité
            glm::cos(glm::radians(20.0f)),  // cutoff intérieur
            glm::cos(glm::radians(35.0f)),  // cutoff extérieur
            1.0f, 0.09f, 0.032f            // atténuation
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

			// RENDER
            FuxEngine::Renderer::Clear();

            cube.GetTransform().SetRotation(glm::vec3(0.0f, glm::degrees(currentFrameTime), 0.0f));
            cube2.GetTransform().SetRotation(glm::vec3(glm::degrees(currentFrameTime), 0.0f, 0.0f));

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
