#include "Graphics/Renderer.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Lighting/Light.h"
#include "Lighting/SpotLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/DirectionalLight.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Mesh.h"

#include <algorithm>
#include <cstddef>
#include <string>

namespace FuxEngine
{
    glm::mat4 Renderer::s_Projection(1.0f);
    float Renderer::s_AmbientStrength = 0.1f;

    namespace
    {
        constexpr int MaxPointLights = 8;
        constexpr int MaxDirectionalLights = 4;
        constexpr int MaxSpotLights = 8;

        void UploadLights(const Scene& scene, Shader& shader)
        {
            const auto& pointLights = scene.GetPointLights();
            const int pointCount = static_cast<int>(
                std::min(pointLights.size(), static_cast<size_t>(MaxPointLights)));
            for (int i = 0; i < pointCount; ++i)
            {
                const PointLight& light = *pointLights[i];
                const std::string prefix = "pointLights[" + std::to_string(i) + "].";
                const glm::vec3& position = light.GetPosition();
                const glm::vec3& color = light.GetColor();
                shader.SetUniform3f(prefix + "position", position.x, position.y, position.z);
                shader.SetUniform3f(prefix + "color", color.x, color.y, color.z);
                shader.SetUniform1f(prefix + "intensity", light.GetIntensity());
                shader.SetUniform1f(prefix + "constant", light.GetConstant());
                shader.SetUniform1f(prefix + "linear", light.GetLinear());
                shader.SetUniform1f(prefix + "quadratic", light.GetQuadratic());
            }
            shader.SetUniform1i("pointLightCount", pointCount);

            const auto& directionalLights = scene.GetDirectionalLights();
            const int directionalCount = static_cast<int>(
                std::min(directionalLights.size(), static_cast<size_t>(MaxDirectionalLights)));
            for (int i = 0; i < directionalCount; ++i)
            {
                const DirectionalLight& light = *directionalLights[i];
                const std::string prefix = "directionalLights[" + std::to_string(i) + "].";
                const glm::vec3& direction = light.GetDirection();
                const glm::vec3& color = light.GetColor();
                shader.SetUniform3f(prefix + "direction", direction.x, direction.y, direction.z);
                shader.SetUniform3f(prefix + "color", color.x, color.y, color.z);
                shader.SetUniform1f(prefix + "intensity", light.GetIntensity());
            }
            shader.SetUniform1i("directionalLightCount", directionalCount);

            const auto& spotLights = scene.GetSpotLights();
            const int spotCount = static_cast<int>(
                std::min(spotLights.size(), static_cast<size_t>(MaxSpotLights)));
            for (int i = 0; i < spotCount; ++i)
            {
                const SpotLight& light = *spotLights[i];
                const std::string prefix = "spotLights[" + std::to_string(i) + "].";
                const glm::vec3& position = light.GetPosition();
                const glm::vec3& direction = light.GetDirection();
                const glm::vec3& color = light.GetColor();
                shader.SetUniform3f(prefix + "position", position.x, position.y, position.z);
                shader.SetUniform3f(prefix + "direction", direction.x, direction.y, direction.z);
                shader.SetUniform3f(prefix + "color", color.x, color.y, color.z);
                shader.SetUniform1f(prefix + "intensity", light.GetIntensity());
                shader.SetUniform1f(prefix + "innerCutoff", light.GetInnerCutoff());
                shader.SetUniform1f(prefix + "outerCutoff", light.GetOuterCutoff());
                shader.SetUniform1f(prefix + "constant", light.GetConstant());
                shader.SetUniform1f(prefix + "linear", light.GetLinear());
                shader.SetUniform1f(prefix + "quadratic", light.GetQuadratic());
            }
            shader.SetUniform1i("spotLightCount", spotCount);
        }

        void PrepareFrameUniforms(
            const Scene& scene,
            const Camera& camera,
            Shader& shader,
            const glm::mat4& projection,
            float ambientStrength
        )
        {
            shader.SetUniformMat4("projection", projection);
            shader.SetUniformMat4("view", camera.GetViewMatrix());
            shader.SetUniform3f(
                "viewPosition",
                camera.GetPosition().x,
                camera.GetPosition().y,
                camera.GetPosition().z
            );
            shader.SetUniform1f("ambientStrength", ambientStrength);
            UploadLights(scene, shader);
        }

        void PrepareEntityUniforms(const Entity& entity, Shader& shader)
        {
            shader.SetUniformMat4(
                "model",
                entity.GetTransform().GetWorldMatrix()
            );
        }
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::SetClearColor(
        float r,
        float g,
        float b,
        float a
    )
    {
        glClearColor(r, g, b, a);
    }

    void Renderer::SetProjection(const glm::mat4& projection)
    {
        s_Projection = projection;
    }

    void Renderer::SetAmbientStrength(float strength)
    {
        s_AmbientStrength = strength;
    }

    void Renderer::Draw(const Mesh& mesh)
    {
        mesh.GetVertexArray().Bind();

        glDrawElements(
            GL_TRIANGLES,
            mesh.GetIndexBuffer().GetCount(),
            GL_UNSIGNED_INT,
            nullptr
        );
    }

    void Renderer::Draw(
        const Scene& scene,
        const Camera& camera
    )
    {
        Shader* preparedShader = nullptr;
        for (const auto& entity : scene.GetEntities())
        {
            if (!entity->IsEnabled())
                continue;

            Material& material = entity->GetMaterial();
            material.Bind();

            Shader& shader = material.GetShader();
            if (preparedShader != &shader)
            {
                PrepareFrameUniforms(scene, camera, shader, s_Projection, s_AmbientStrength);
                preparedShader = &shader;
            }
            PrepareEntityUniforms(*entity, shader);

            Draw(entity->GetMesh());
        }
    }
}
