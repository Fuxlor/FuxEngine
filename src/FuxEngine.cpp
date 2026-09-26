#include "Core/Window.h"
#include "Core/Camera.h"
#include "Core/CameraController.h"
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

        // CAMERAS: press 1 or 2 to switch
        FuxEngine::CameraController cameraController(window.GetNativeWindow());
        FuxEngine::Camera& freeCamera = cameraController.CreateCamera(
            glm::vec3(0.0f, 0.0f, 3.0f)
        );
        freeCamera.SetPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
        freeCamera.SetMovementSpeed(2.5f);
        freeCamera.SetMouseSensitivity(0.1f);

        FuxEngine::Camera& overviewCamera = cameraController.CreateCamera(
            glm::vec3(0.0f, 6.0f, 8.0f)
        );
        overviewCamera.SetPerspective(55.0f, 1280.0f / 720.0f, 0.1f, 200.0f);
        overviewCamera.SetYawPitch(-90.0f, -35.0f);
        overviewCamera.SetMovementSpeed(5.0f);
        overviewCamera.SetMouseSensitivity(0.08f);

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

            // CONTROLS: WASD moves the active camera; keys 1-9 select a camera.
            cameraController.Update(deltaTime);

            // EXIT
            if (glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(nativeWindow, true);

			// RENDER
            FuxEngine::Renderer::Clear();

            cube.GetTransform().SetRotation(glm::vec3(0.0f, glm::degrees(currentFrameTime), 0.0f));
            cube2.GetTransform().SetRotation(glm::vec3(glm::degrees(currentFrameTime), 0.0f, 0.0f));

            FuxEngine::Renderer::Draw(scene, cameraController.GetActiveCamera());

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
