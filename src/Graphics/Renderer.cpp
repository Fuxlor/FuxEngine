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
#include "Graphics/PostProcessor.h"
#include "Graphics/ShadowRenderer.h"

#include <algorithm>
#include <cstddef>
#include <vector>
#include <string>

namespace FuxEngine
{
    float Renderer::s_AmbientStrength = 0.1f;
    bool Renderer::s_Wireframe = false;

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
            float ambientStrength,
            bool gammaCorrection,
            ShadowRenderer* shadowRenderer
        )
        {
            shader.SetUniformMat4("projection", camera.GetProjectionMatrix());
            shader.SetUniformMat4("view", camera.GetViewMatrix());
            shader.SetUniform3f(
                "viewPosition",
                camera.GetPosition().x,
                camera.GetPosition().y,
                camera.GetPosition().z
            );
            shader.SetUniform1f("ambientStrength", ambientStrength);
            shader.SetUniform1i("gammaCorrection", gammaCorrection ? 1 : 0);
            const bool hasShadowMap = shadowRenderer && shadowRenderer->HasShadowMap();
            shader.SetUniform1i("hasShadowMap", hasShadowMap ? 1 : 0);
            shader.SetUniform1i("shadowMap", 3);
            if (shadowRenderer)
            {
                shader.SetUniformMat4("lightSpaceMatrix", shadowRenderer->GetLightSpaceMatrix());
                if (hasShadowMap)
                    shadowRenderer->BindTexture(3);
            }
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

    void Renderer::SetAmbientStrength(float strength)
    {
        s_AmbientStrength = strength;
    }

    void Renderer::SetWireframe(bool enabled)
    {
        s_Wireframe = enabled;
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
        const Camera& camera,
        PostProcessor* postProcessor,
        ShadowRenderer* shadowRenderer
    )
    {
        if (shadowRenderer)
            shadowRenderer->Render(scene);
        if (postProcessor)
            postProcessor->BeginScene();

        std::vector<Entity*> opaqueEntities;
        std::vector<Entity*> transparentEntities;
        for (const auto& entity : scene.GetEntities())
        {
            if (!entity->IsEnabled())
                continue;

            if (entity->GetMaterial().IsTransparent())
                transparentEntities.push_back(entity.get());
            else
                opaqueEntities.push_back(entity.get());
        }

        const glm::vec3 cameraPosition = camera.GetPosition();
        std::sort(
            transparentEntities.begin(),
            transparentEntities.end(),
            [&cameraPosition](const Entity* left, const Entity* right)
            {
                const glm::vec3 leftPosition(left->GetTransform().GetWorldMatrix()[3]);
                const glm::vec3 rightPosition(right->GetTransform().GetWorldMatrix()[3]);
                const glm::vec3 leftDelta = leftPosition - cameraPosition;
                const glm::vec3 rightDelta = rightPosition - cameraPosition;
                return glm::dot(leftDelta, leftDelta) > glm::dot(rightDelta, rightDelta);
            }
        );

        Shader* preparedShader = nullptr;
        auto drawEntity = [&](Entity& entity)
        {
            Material& material = entity.GetMaterial();
            material.Bind();

            Shader& shader = material.GetShader();
            if (preparedShader != &shader)
            {
                PrepareFrameUniforms(
                    scene,
                    camera,
                    shader,
                    s_AmbientStrength,
                    postProcessor == nullptr,
                    shadowRenderer
                );
                preparedShader = &shader;
            }
            PrepareEntityUniforms(entity, shader);

            if (material.IsDoubleSided())
                glDisable(GL_CULL_FACE);
            else
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
            }

            Draw(entity.GetMesh());
        };

        glPolygonMode(GL_FRONT_AND_BACK, s_Wireframe ? GL_LINE : GL_FILL);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        for (Entity* entity : opaqueEntities)
            drawEntity(*entity);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        for (Entity* entity : transparentEntities)
            drawEntity(*entity);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (postProcessor)
            postProcessor->Present();
    }
}
