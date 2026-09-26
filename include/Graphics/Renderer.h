#pragma once

#include <glad/glad.h>

#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/Camera.h"

namespace FuxEngine
{
    class PostProcessor;
    class ShadowRenderer;
    class Mesh;

    class Renderer
    {
    public:
        static void Clear();

        static void SetAmbientStrength(float strength);
        static void SetWireframe(bool enabled);

        static void SetClearColor(
            float r,
            float g,
            float b,
            float a
        );

        static void Draw(const Mesh& mesh);

        static void Draw(
            const Scene& scene,
            const Camera& camera,
            PostProcessor* postProcessor = nullptr,
            ShadowRenderer* shadowRenderer = nullptr
        );

    private:
        static float s_AmbientStrength;
        static bool s_Wireframe;
    };
}
