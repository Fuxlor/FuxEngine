#include "Core/Window.h"
#include "Core/Camera.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "Graphics/Renderer.h"
#include "Resources/MaterialManager.h"
#include "Resources/ObjLoader.h"
#include "Resources/ShaderManager.h"
#include "Resources/TextureManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

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

        FuxEngine::ShaderManager shaderManager;
        FuxEngine::TextureManager textureManager;
        FuxEngine::MaterialManager materialManager(shaderManager, textureManager);
        FuxEngine::Material& material = materialManager.Load("basic");
        FuxEngine::Renderer::SetAmbientStrength(0.1f);

        std::unique_ptr<FuxEngine::Mesh> mesh = FuxEngine::ObjLoader::Load("cube.obj");
        std::unique_ptr<FuxEngine::Mesh> body = FuxEngine::ObjLoader::Load("FinalBaseMesh.obj");

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
        FuxEngine::Entity& cube = scene.CreateEntity(*mesh, material, "CubeLeft");
        FuxEngine::Entity& cube2 = scene.CreateEntity(*mesh, material, "CubeRight");
        FuxEngine::Entity& bodyEntity = scene.CreateEntity(*body, material, "Body");
        cube.GetTransform().SetPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
        cube2.GetTransform().SetPosition(glm::vec3(1.5f, 0.0f, 0.0f));
        bodyEntity.GetTransform().SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
        bodyEntity.GetTransform().SetScale(glm::vec3(0.1f));
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
