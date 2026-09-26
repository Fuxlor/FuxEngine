#pragma once

#include <glad/glad.h>

#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/Camera.h"

namespace FuxEngine
{
    class Mesh;

    class Renderer
    {
    public:
        static void Clear();

        static void SetProjection(const glm::mat4& projection);
        static void SetAmbientStrength(float strength);

        static void SetClearColor(
            float r,
            float g,
            float b,
            float a
        );

        static void Draw(const Mesh& mesh);

        static void Draw(
            const Scene& scene,
            const Camera& camera
        );

    private:
        static glm::mat4 s_Projection;
        static float s_AmbientStrength;
    };
}
