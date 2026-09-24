#pragma once

#include <glad/glad.h>

namespace FuxEngine
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const void* data, unsigned int size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

        unsigned int GetID() const;

    private:
        unsigned int m_ID = 0;
    };
}