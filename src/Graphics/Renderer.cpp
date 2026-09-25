#include "Graphics/Renderer.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Light.h"
#include "Core/SpotLight.h"
#include "Core/PointLight.h"
#include "Core/DirectionalLight.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Mesh.h"

namespace FuxEngine
{
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
        const auto& lights = scene.GetLights();

        for (const auto& entity : scene.GetEntities())
        {
            Material& material = entity->GetMaterial();
            material.Bind();

            Shader& shader = material.GetShader();

            shader.SetUniform3f(
                "viewPosition",
                camera.GetPosition().x,
                camera.GetPosition().y,
                camera.GetPosition().z
            );

            shader.SetUniformMat4(
                "model",
                entity->GetTransform().GetMatrix()
            );

            int pointLightCount = 0;
            int directionalLightCount = 0;
            int spotLightCount = 0;

            for (const auto& light : lights)
            {
                switch (light->GetType())
                {
                case LightType::Point:
                {
                    if (pointLightCount >= 8)
                        break;

                    const PointLight& pointLight =
                        static_cast<const PointLight&>(*light);

                    std::string prefix =
                        "pointLights[" +
                        std::to_string(pointLightCount) +
                        "].";

                    shader.SetUniform3f(
                        prefix + "position",
                        pointLight.GetPosition().x,
                        pointLight.GetPosition().y,
                        pointLight.GetPosition().z
                    );

                    shader.SetUniform3f(
                        prefix + "color",
                        pointLight.GetColor().x,
                        pointLight.GetColor().y,
                        pointLight.GetColor().z
                    );

                    shader.SetUniform1f(
                        prefix + "intensity",
                        pointLight.GetIntensity()
                    );

                    shader.SetUniform1f(
                        prefix + "constant",
                        pointLight.GetConstant()
                    );

                    shader.SetUniform1f(
                        prefix + "linear",
                        pointLight.GetLinear()
                    );

                    shader.SetUniform1f(
                        prefix + "quadratic",
                        pointLight.GetQuadratic()
                    );

                    pointLightCount++;
                    break;
                }

                case LightType::Directional:
                {
                    if (directionalLightCount >= 4)
                        break;

                    // On remplira ça avec DirectionalLight.
                    // TODO: Implement DirectionalLight
                    directionalLightCount++;
                    break;
                }

                case LightType::Spot:
                {
                    if (spotLightCount >= 8)
                        break;

                    // On remplira ça avec SpotLight.
                    // TODO: Implement SpotLight
                    spotLightCount++;
                    break;
                }
                }
            }

            shader.SetUniform1i(
                "pointLightCount",
                pointLightCount
            );

            shader.SetUniform1i(
                "directionalLightCount",
                directionalLightCount
            );

            shader.SetUniform1i(
                "spotLightCount",
                spotLightCount
            );

            Draw(entity->GetMesh());
        }
    }
}