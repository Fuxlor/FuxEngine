#pragma once

#include <vector>
#include <initializer_list>
#include <string>

namespace FuxEngine
{
    enum class ShaderDataType
    {
        None = 0,

        Float,
        Float2,
        Float3,
        Float4,

        Int,
        Int2,
        Int3,
        Int4,

        Bool
    };

    inline unsigned int ShaderDataTypeSize(
        ShaderDataType type
    )
    {
        switch (type)
        {
        case ShaderDataType::Float:
            return sizeof(float);

        case ShaderDataType::Float2:
            return sizeof(float) * 2;

        case ShaderDataType::Float3:
            return sizeof(float) * 3;

        case ShaderDataType::Float4:
            return sizeof(float) * 4;

        case ShaderDataType::Int:
            return sizeof(int);

        case ShaderDataType::Int2:
            return sizeof(int) * 2;

        case ShaderDataType::Int3:
            return sizeof(int) * 3;

        case ShaderDataType::Int4:
            return sizeof(int) * 4;

        case ShaderDataType::Bool:
            return sizeof(bool);

        case ShaderDataType::None:
            return 0;
        }

        return 0;
    }

    inline unsigned int ShaderDataTypeComponentCount(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:  return 1;
        case ShaderDataType::Float2: return 2;
        case ShaderDataType::Float3: return 3;
        case ShaderDataType::Float4: return 4;

        case ShaderDataType::Int:  return 1;
        case ShaderDataType::Int2: return 2;
        case ShaderDataType::Int3: return 3;
        case ShaderDataType::Int4: return 4;

        case ShaderDataType::Bool: return 1;

        case ShaderDataType::None: return 0;
        }

        return 0;
    }

    inline unsigned int ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
            return GL_FLOAT;

        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
            return GL_INT;

        case ShaderDataType::Bool:
            return GL_BOOL;

        case ShaderDataType::None:
            return 0;
        }

        return 0;
    }

    struct BufferElement
    {
        std::string Name;

        ShaderDataType Type;

        unsigned int Size;
        unsigned int Offset;

        bool Normalized;

        BufferElement(
            ShaderDataType type,
            const std::string& name,
            bool normalized = false
        )
            : Name(name),
            Type(type),
            Size(ShaderDataTypeSize(type)),
            Offset(0),
            Normalized(normalized)
        {}
    };

    class BufferLayout
    {
        public:
            BufferLayout(
                std::initializer_list<BufferElement> elements
            )
                : m_Elements(elements)
            {
                CalculateOffsetsAndStride();
            }

            const std::vector<BufferElement>& GetElements() const
            {
                return m_Elements;
            }

            unsigned int GetStride() const
            {
                return m_Stride;
            }

        private:
            void CalculateOffsetsAndStride()
            {
                unsigned int offset = 0;

                m_Stride = 0;

                for (BufferElement& element : m_Elements)
                {
                    element.Offset = offset;

                    offset += element.Size;

                    m_Stride += element.Size;
                }
            }

        private:
            std::vector<BufferElement> m_Elements;
            unsigned int m_Stride = 0;
    };
}