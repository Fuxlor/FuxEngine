#include "Physics/PhysicsDebugRenderer.h"

#include "Core/Camera.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Physics/PhysicsWorld.h"

#include <algorithm>
#include <array>
#include <vector>

namespace FuxEngine
{
    namespace
    {
        struct DebugVertex
        {
            float position[3];
            float color[3];
        };

        void AddLine(std::vector<DebugVertex>& vertices, const glm::vec3& from,
                     const glm::vec3& to, const glm::vec3& color)
        {
            vertices.push_back({{from.x, from.y, from.z}, {color.x, color.y, color.z}});
            vertices.push_back({{to.x, to.y, to.z}, {color.x, color.y, color.z}});
        }
    }

    PhysicsDebugRenderer::PhysicsDebugRenderer(Shader& shader) : m_Shader(shader)
    {
        glGenVertexArrays(1, &m_VertexArray);
        glGenBuffers(1, &m_VertexBuffer);
        glBindVertexArray(m_VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex),
                              reinterpret_cast<const void*>(sizeof(float) * 3));
        glBindVertexArray(0);
    }

    PhysicsDebugRenderer::~PhysicsDebugRenderer()
    {
        if (m_VertexBuffer)
            glDeleteBuffers(1, &m_VertexBuffer);
        if (m_VertexArray)
            glDeleteVertexArrays(1, &m_VertexArray);
    }

    void PhysicsDebugRenderer::Draw(const Scene& scene, const PhysicsWorld& world, const Camera& camera) const
    {
        std::vector<DebugVertex> vertices;
        constexpr glm::vec3 boxColor(0.15f, 1.0f, 0.25f);
        constexpr glm::vec3 sphereColor(0.1f, 0.85f, 1.0f);
        constexpr glm::vec3 normalColor(1.0f, 0.15f, 0.1f);

        for (const auto& body : world.GetBodies())
        {
            const Entity* entity = scene.FindEntity(body->GetEntityId());
            if (!entity)
                continue;
            const glm::vec3 center = entity->GetTransform().GetPosition();
            const glm::vec3 scale = glm::abs(entity->GetTransform().GetScale());

            if (body->GetShape() == CollisionShape::AABB)
            {
                const glm::vec3 half = body->GetDimensions() * scale;
                std::array<glm::vec3, 8> corners;
                for (unsigned int i = 0; i < corners.size(); ++i)
                {
                    corners[i] = center + glm::vec3(
                        (i & 1) ? half.x : -half.x,
                        (i & 2) ? half.y : -half.y,
                        (i & 4) ? half.z : -half.z
                    );
                }
                for (unsigned int i = 0; i < corners.size(); ++i)
                {
                    for (unsigned int axis = 0; axis < 3; ++axis)
                    {
                        const unsigned int bit = 1u << axis;
                        if ((i & bit) == 0)
                            AddLine(vertices, corners[i], corners[i | bit], boxColor);
                    }
                }
            }
            else
            {
                const float radius = body->GetDimensions().x * std::max(scale.x, std::max(scale.y, scale.z));
                constexpr int segments = 32;
                constexpr float tau = 6.28318530718f;
                for (int plane = 0; plane < 3; ++plane)
                {
                    for (int i = 0; i < segments; ++i)
                    {
                        const float angle0 = tau * static_cast<float>(i) / segments;
                        const float angle1 = tau * static_cast<float>(i + 1) / segments;
                        glm::vec3 p0(0.0f), p1(0.0f);
                        const int firstAxis = (plane + 1) % 3;
                        const int secondAxis = (plane + 2) % 3;
                        p0[firstAxis] = std::cos(angle0) * radius;
                        p0[secondAxis] = std::sin(angle0) * radius;
                        p1[firstAxis] = std::cos(angle1) * radius;
                        p1[secondAxis] = std::sin(angle1) * radius;
                        AddLine(vertices, center + p0, center + p1, sphereColor);
                    }
                }
            }
        }

        for (const PhysicsContact& contact : world.GetLastContacts())
            AddLine(vertices, contact.point, contact.point + contact.normal * 0.75f, normalColor);

        if (vertices.empty())
            return;

        m_Shader.Bind();
        m_Shader.SetUniformMat4("projection", camera.GetProjectionMatrix());
        m_Shader.SetUniformMat4("view", camera.GetViewMatrix());

        glBindVertexArray(m_VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(DebugVertex)),
                     vertices.data(), GL_DYNAMIC_DRAW);
        glDepthFunc(GL_LEQUAL);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
        glLineWidth(1.0f);
        glDepthFunc(GL_LESS);
        glBindVertexArray(0);
    }
}
