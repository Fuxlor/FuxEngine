#pragma once

#include <glad/glad.h>
#include "Core/Entity.h"
#include "Graphics/Shader.h"

namespace FuxEngine
{
    class VertexArray;
	class IndexBuffer;
    class Mesh;

    class Renderer
    {
    public:
        static void Clear();
        static void SetClearColor(
            float r,
            float g,
            float b,
            float a
        );

        static void Draw(const Mesh& mesh);

        static void Draw(Entity& entity);
    };
}