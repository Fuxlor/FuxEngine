#include "Core/Window.h"
#include "Core/Camera.h"
#include "Input/CameraController.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "Lighting/DirectionalLight.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"
#include "Graphics/PostProcessor.h"
#include "Graphics/ShadowRenderer.h"
#include "Resources/MaterialManager.h"
#include "Resources/ObjLoader.h"
#include "Resources/ShaderManager.h"
#include "Resources/TextureManager.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsDebugRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>
#include <cmath>
#include <algorithm>
#include <vector>

namespace
{
    std::unique_ptr<FuxEngine::Mesh> CreateSphereMesh(unsigned int segments = 24, unsigned int rings = 16)
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        vertices.reserve((rings + 1) * (segments + 1) * 11);

        for (unsigned int ring = 0; ring <= rings; ++ring)
        {
            const float v = static_cast<float>(ring) / static_cast<float>(rings);
            const float phi = v * glm::pi<float>();
            for (unsigned int segment = 0; segment <= segments; ++segment)
            {
                const float u = static_cast<float>(segment) / static_cast<float>(segments);
                const float theta = u * glm::two_pi<float>();
                const glm::vec3 normal(
                    std::sin(phi) * std::cos(theta),
                    std::cos(phi),
                    std::sin(phi) * std::sin(theta)
                );
                const glm::vec3 position = normal * 0.5f;
                vertices.insert(vertices.end(), {
                    position.x, position.y, position.z,
                    normal.x, normal.y, normal.z,
                    1.0f, 1.0f, 1.0f,
                    u, v
                });
            }
        }

        for (unsigned int ring = 0; ring < rings; ++ring)
        {
            for (unsigned int segment = 0; segment < segments; ++segment)
            {
                const unsigned int current = ring * (segments + 1) + segment;
                const unsigned int nextRing = current + segments + 1;
                indices.insert(indices.end(), {
                    current, current + 1, nextRing,
                    current + 1, nextRing + 1, nextRing
                });
            }
        }

        return std::make_unique<FuxEngine::Mesh>(
            vertices.data(),
            static_cast<unsigned int>(vertices.size() * sizeof(float)),
            indices.data(),
            static_cast<unsigned int>(indices.size())
        );
    }
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
        FuxEngine::Shader& postProcessShader = shaderManager.Load("postprocess");
        FuxEngine::PostProcessor postProcessor(postProcessShader, window.GetNativeWindow());
        postProcessor.SetExposure(0.7f);
        FuxEngine::Shader& depthShader = shaderManager.Load("depth");
        FuxEngine::ShadowRenderer shadowRenderer(depthShader);
        FuxEngine::Shader& physicsDebugShader = shaderManager.Load("physics_debug");
        FuxEngine::PhysicsDebugRenderer physicsDebugRenderer(physicsDebugShader);
        FuxEngine::Renderer::SetAmbientStrength(0.025f);

        std::unique_ptr<FuxEngine::Mesh> mesh = FuxEngine::ObjLoader::Load("cube.obj");
        std::unique_ptr<FuxEngine::Mesh> sphereMesh = CreateSphereMesh();

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
        cameraController.SetActiveCamera(1);

        glEnable(GL_DEPTH_TEST);

		// PHYSICS PLAYGROUND: the cases are arranged side by side over one floor.
        FuxEngine::Scene scene;
        FuxEngine::PhysicsWorld physics(glm::vec3(0.0f, -9.81f, 0.0f));

        FuxEngine::Entity& floor = scene.CreateEntity(*mesh, material, "Ground");
        floor.GetTransform().SetPosition(glm::vec3(0.0f, -1.5f, 0.0f));
        floor.GetTransform().SetScale(glm::vec3(20.0f, 0.5f, 4.0f));
        auto& floorBody = physics.CreateBody(floor, FuxEngine::CollisionShape::AABB, 0.0f);
        floorBody.SetRestitution(0.9f);

        // Cube + floor: drop the cube from above.
        FuxEngine::Entity& fallingCube = scene.CreateEntity(*mesh, material, "Cube_Floor");
        fallingCube.GetTransform().SetPosition(glm::vec3(-5.0f, 2.0f, 0.0f));
        auto& fallingCubeBody = physics.CreateBody(fallingCube, FuxEngine::CollisionShape::AABB, 1.0f);

        // Cube + cube: initial velocities point towards one another.
        FuxEngine::Entity& leftCube = scene.CreateEntity(*mesh, material, "CubePair_Left");
        FuxEngine::Entity& rightCube = scene.CreateEntity(*mesh, material, "CubePair_Right");
        leftCube.GetTransform().SetPosition(glm::vec3(-3.8f, -0.75f, 0.0f));
        rightCube.GetTransform().SetPosition(glm::vec3(-1.0f, -0.75f, 0.0f));
        auto& leftCubeBody = physics.CreateBody(leftCube, FuxEngine::CollisionShape::AABB, 1.0f);
        auto& rightCubeBody = physics.CreateBody(rightCube, FuxEngine::CollisionShape::AABB, 1.0f);
        leftCubeBody.SetVelocity(glm::vec3(1.0f, 0.0f, 0.0f));
        rightCubeBody.SetVelocity(glm::vec3(-1.0f, 0.0f, 0.0f));
        leftCubeBody.SetRestitution(0.8f);
        rightCubeBody.SetRestitution(0.8f);
        leftCubeBody.SetFriction(0.001f);
        rightCubeBody.SetFriction(0.001f);

        // Sphere + sphere: use a generated sphere mesh and sphere colliders.
        FuxEngine::Entity& leftSphere = scene.CreateEntity(*sphereMesh, material, "SpherePair_Left");
        FuxEngine::Entity& rightSphere = scene.CreateEntity(*sphereMesh, material, "SpherePair_Right");
        leftSphere.GetTransform().SetPosition(glm::vec3(0.0f, -0.75f, 0.0f));
        rightSphere.GetTransform().SetPosition(glm::vec3(2.8f, -0.75f, 0.0f));
        auto& leftSphereBody = physics.CreateBody(leftSphere, FuxEngine::CollisionShape::Sphere, 1.0f);
        auto& rightSphereBody = physics.CreateBody(rightSphere, FuxEngine::CollisionShape::Sphere, 1.0f);
        leftSphereBody.SetVelocity(glm::vec3(1.5f, 0.0f, 0.0f));
        rightSphereBody.SetVelocity(glm::vec3(-1.5f, 0.0f, 0.0f));
        leftSphereBody.SetRestitution(0.8f);
        rightSphereBody.SetRestitution(0.8f);
        leftSphereBody.SetFriction(0.001f);
        rightSphereBody.SetFriction(0.001f);

        // Rebound and friction demonstrations.
        FuxEngine::Entity& reboundCube = scene.CreateEntity(*mesh, material, "Rebound");
        reboundCube.GetTransform().SetPosition(glm::vec3(3.5f, 0.0f, 0.0f));
        auto& reboundBody = physics.CreateBody(reboundCube, FuxEngine::CollisionShape::AABB, 1.0f);
        reboundBody.SetVelocity(glm::vec3(0.0f, -1.0f, 0.0f));
        reboundBody.SetRestitution(0.9f);

        FuxEngine::Entity& frictionCube = scene.CreateEntity(*mesh, material, "Friction");
        frictionCube.GetTransform().SetPosition(glm::vec3(5.5f, -0.75f, 0.0f));
        auto& frictionBody = physics.CreateBody(frictionCube, FuxEngine::CollisionShape::AABB, 1.0f);
        frictionBody.SetVelocity(glm::vec3(6.0f, 0.0f, 0.0f));
        frictionBody.SetFriction(2.0f);
        scene.CreateLight<FuxEngine::PointLight>(
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(1.0f, 0.85f, 0.65f),
            1.0f,
            1.0f,
			0.12f,
			0.08f
        );
        scene.CreateLight<FuxEngine::SpotLight>(
            glm::vec3(-2.0f, -2.0f, 0.0f), // position sous les cubes
            glm::vec3(0.0f, 1.0f, 0.0f),  // direction vers le haut
            glm::vec3(1.0f, 0.85f, 0.65f), // couleur chaude
            0.35f,                          // intensité
            glm::cos(glm::radians(20.0f)),  // cutoff intérieur
            glm::cos(glm::radians(35.0f)),  // cutoff extérieur
            1.0f, 0.12f, 0.08f             // atténuation
        );
        /*scene.CreateLight<FuxEngine::DirectionalLight>(
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(1.0f),
            0.2f
        );*/

        // MAIN LOOP
        float lastFrameTime = 0.0f;
        float physicsAccumulator = 0.0f;
        constexpr float physicsTimeStep = 1.0f / 120.0f;
        bool wireframe = false;
        bool wireframeKeyWasPressed = false;
        bool physicsDebugEnabled = false;
        bool physicsDebugKeyWasPressed = false;
        while (!window.ShouldClose())
        {
			// TIME
            float currentFrameTime = static_cast<float>(glfwGetTime());
            float deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            GLFWwindow* nativeWindow = window.GetNativeWindow();

            const bool wireframeKeyIsPressed =
                glfwGetKey(nativeWindow, GLFW_KEY_F) == GLFW_PRESS;
            if (wireframeKeyIsPressed && !wireframeKeyWasPressed)
            {
                wireframe = !wireframe;
                FuxEngine::Renderer::SetWireframe(wireframe);
            }
            wireframeKeyWasPressed = wireframeKeyIsPressed;

            const bool physicsDebugKeyIsPressed =
                glfwGetKey(nativeWindow, GLFW_KEY_F2) == GLFW_PRESS;
            if (physicsDebugKeyIsPressed && !physicsDebugKeyWasPressed)
                physicsDebugEnabled = !physicsDebugEnabled;
            physicsDebugKeyWasPressed = physicsDebugKeyIsPressed;

            // CONTROLS: WASD moves the active camera; keys 1-9 select a camera.
            cameraController.Update(deltaTime);

            // EXIT
            if (glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(nativeWindow, true);

			// RENDER
            FuxEngine::Renderer::Clear();

            physicsAccumulator += std::min(deltaTime, 0.25f);
            while (physicsAccumulator >= physicsTimeStep)
            {
                fallingCubeBody.AddTorque(glm::vec3(0.0f, 0.0f, 0.2f));
                physics.Step(scene, physicsTimeStep);
                physicsAccumulator -= physicsTimeStep;
            }

            FuxEngine::Renderer::Draw(
                scene,
                cameraController.GetActiveCamera(),
                &postProcessor,
                &shadowRenderer
            );
            if (physicsDebugEnabled)
                physicsDebugRenderer.Draw(scene, physics, cameraController.GetActiveCamera());

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
