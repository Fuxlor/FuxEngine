#include "Graphics/Renderer.h"
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

        if (lights.empty())
            return;

        const Light& light = *lights[0];

        for (const auto& entity : scene.GetEntities())
        {
            Material& material = entity->GetMaterial();

            material.Bind();

            Shader& shader = material.GetShader();

            shader.SetUniform3f(
                "lightPosition",
                light.GetPosition().x,
                light.GetPosition().y,
                light.GetPosition().z
            );

            shader.SetUniform3f(
                "lightColor",
                light.GetColor().x * light.GetIntensity(),
                light.GetColor().y * light.GetIntensity(),
                light.GetColor().z * light.GetIntensity()
            );

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

            Draw(entity->GetMesh());
        }
    }
}