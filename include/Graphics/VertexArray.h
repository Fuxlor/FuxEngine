#pragma once

#include <glad/glad.h>
#include "Graphics/BufferLayout.h"

namespace FuxEngine
{
    class VertexArray
    {
        public:
            VertexArray();
            ~VertexArray();

            void Bind() const;
            void Unbind() const;

            void SetLayout(const BufferLayout& layout);

        private:
            void AddAttribute(
                unsigned int index,
                int componentCount,
                unsigned int type,
                bool normalized,
                int stride,
                const void* offset
            );

        private:
            unsigned int m_RendererID;
    };
}