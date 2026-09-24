#pragma once

#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

#include <optional>

namespace FuxEngine
{
    class Mesh
    {
    public:
        Mesh(
            const float* vertices,
            unsigned int vertexSize,
            const unsigned int* indices,
            unsigned int indexCount
        );

        ~Mesh() = default;

        void SetLayout();

        const VertexArray& GetVertexArray() const;
        const IndexBuffer& GetIndexBuffer() const;

    private:
        VertexArray m_VertexArray;
        VertexBuffer m_VertexBuffer;
        std::optional<IndexBuffer> m_IndexBuffer;
    };
}