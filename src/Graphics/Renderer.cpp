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

    void Renderer::Draw(Entity& entity)
    {
        Material& material = entity.GetMaterial();

        material.Bind();

        material.GetShader().SetUniformMat4(
            "model",
            entity.GetTransform().GetMatrix()
        );

        Draw(entity.GetMesh());
    }
}