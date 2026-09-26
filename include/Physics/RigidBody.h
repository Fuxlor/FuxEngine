#pragma once

#include "Core/Entity.h"

namespace FuxEngine
{
    enum class CollisionShape
    {
        AABB,
        Sphere
    };

    // Mass zero creates a static body. Shape dimensions are local half-extents/radius.
    class RigidBody
    {
    public:
        EntityId GetEntityId() const;
        CollisionShape GetShape() const;
        float GetMass() const;
        const glm::vec3& GetDimensions() const;
        const glm::vec3& GetVelocity() const;
        const glm::vec3& GetAngularVelocity() const;
        // Principal inertia values in the body's local axes.
        const glm::vec3& GetInertia() const;
        const glm::vec3& GetInverseInertiaLocal() const;
        float GetRestitution() const;
        float GetFriction() const;

        void SetVelocity(const glm::vec3& velocity);
        // Angular velocity is in world-space radians per second.
        void SetAngularVelocity(const glm::vec3& angularVelocity);
        void SetRestitution(float restitution);
        void SetFriction(float friction);
        void AddForce(const glm::vec3& force);
        // Torque is accumulated in world space and consumed by the next physics step.
        void AddTorque(const glm::vec3& torque);
        void ApplyImpulse(const glm::vec3& impulse);

    private:
        friend class PhysicsWorld;
        RigidBody(EntityId entityId, CollisionShape shape, float mass, glm::vec3 dimensions,
                  const glm::vec3& transformScale);

        EntityId m_EntityId;
        CollisionShape m_Shape;
        float m_Mass;
        glm::vec3 m_Dimensions;
        glm::vec3 m_Velocity{0.0f};
        glm::vec3 m_AngularVelocity{0.0f};
        glm::vec3 m_Force{0.0f};
        glm::vec3 m_Torque{0.0f};
        glm::vec3 m_Inertia{0.0f};
        glm::vec3 m_InverseInertia{0.0f};
        float m_Restitution = 0.0f;
        float m_Friction = 0.5f;
    };
}
