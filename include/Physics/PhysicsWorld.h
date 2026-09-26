#pragma once

#include "Physics/RigidBody.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace FuxEngine
{
    class Scene;

    struct PhysicsContact
    {
        glm::vec3 point;
        glm::vec3 normal;
    };

    class PhysicsWorld
    {
    public:
        explicit PhysicsWorld(glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f));

        RigidBody& CreateBody(Entity& entity, CollisionShape shape, float mass,
                              const glm::vec3& dimensions = glm::vec3(0.5f));
        bool RemoveBody(EntityId entityId);
        RigidBody* FindBody(EntityId entityId);
        const std::vector<std::unique_ptr<RigidBody>>& GetBodies() const;
        const std::vector<PhysicsContact>& GetLastContacts() const;
        void SetGravity(const glm::vec3& gravity);
        const glm::vec3& GetGravity() const;

        // Semi-implicit Euler; call with a fixed delta time in seconds.
        void Step(Scene& scene, float deltaTime);

    private:
        glm::vec3 m_Gravity;
        std::vector<std::unique_ptr<RigidBody>> m_Bodies;
        std::vector<PhysicsContact> m_LastContacts;
    };
}
