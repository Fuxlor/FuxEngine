#pragma once

#include "Graphics/Shader.h"

namespace FuxEngine
{
    class Camera;
    class PhysicsWorld;
    class Scene;

    class PhysicsDebugRenderer
    {
    public:
        explicit PhysicsDebugRenderer(Shader& shader);
        ~PhysicsDebugRenderer();

        PhysicsDebugRenderer(const PhysicsDebugRenderer&) = delete;
        PhysicsDebugRenderer& operator=(const PhysicsDebugRenderer&) = delete;

        void Draw(const Scene& scene, const PhysicsWorld& world, const Camera& camera) const;

    private:
        Shader& m_Shader;
        unsigned int m_VertexArray = 0;
        unsigned int m_VertexBuffer = 0;
    };
}
