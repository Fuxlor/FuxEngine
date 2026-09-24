#include "Graphics/Mesh.h"
#include "Graphics/BufferLayout.h"

namespace FuxEngine
{
    Mesh::Mesh(
        const float* vertices,
        unsigned int vertexSize,
        const unsigned int* indices,
        unsigned int indexCount
    )
        : m_VertexArray(),
          m_VertexBuffer(vertices, vertexSize)
    {
        m_VertexArray.Bind();
        m_VertexBuffer.Bind();

        SetLayout();

        m_IndexBuffer.emplace(
            indices,
            indexCount
        );
    }

    void Mesh::SetLayout()
    {
        BufferLayout layout = {
            { ShaderDataType::Float2, "aPos" },
            { ShaderDataType::Float3, "aColor" },
            { ShaderDataType::Float2, "aTexCoord" }
        };

        m_VertexArray.SetLayout(layout);
    }

    const VertexArray& Mesh::GetVertexArray() const
    {
        return m_VertexArray;
    }

    const IndexBuffer& Mesh::GetIndexBuffer() const
    {
        return *m_IndexBuffer;
    }
}