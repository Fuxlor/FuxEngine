#pragma once

#include <glad/glad.h>

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
    };
}