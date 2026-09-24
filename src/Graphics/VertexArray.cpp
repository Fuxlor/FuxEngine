#include "Graphics/VertexArray.h"

namespace FuxEngine
{
    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddAttribute(
        unsigned int index,
        int count,
        unsigned int type,
        bool normalized,
        int stride,
        const void* offset
    )
    {
        glVertexAttribPointer(
            index,
            count,
            type,
            normalized ? GL_TRUE : GL_FALSE,
            stride,
            offset
        );

        glEnableVertexAttribArray(index);
    }

    void VertexArray::SetLayout(const BufferLayout& layout)
    {
        Bind();

        unsigned int index = 0;

        for (const BufferElement& element : layout.GetElements())
        {
            AddAttribute(
                index,
                static_cast<int>(ShaderDataTypeComponentCount(element.Type)),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized,
                static_cast<int>(layout.GetStride()),
                reinterpret_cast<const void*>(
                    static_cast<uintptr_t>(element.Offset)
                    )
            );

            index++;
        }
    }
}