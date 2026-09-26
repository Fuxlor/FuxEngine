#include "Physics/RigidBody.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace FuxEngine
{
    RigidBody::RigidBody(EntityId entityId, CollisionShape shape, float mass, glm::vec3 dimensions,
                         const glm::vec3& transformScale)
        : m_EntityId(entityId), m_Shape(shape), m_Mass(mass), m_Dimensions(dimensions)
    {
        if (!std::isfinite(mass) || mass < 0.0f)
            throw std::invalid_argument("Rigid body mass must be finite and non-negative");
        if (glm::any(glm::lessThanEqual(dimensions, glm::vec3(0.0f))) ||
            glm::any(glm::isnan(dimensions)) || glm::any(glm::isinf(dimensions)))
            throw std::invalid_argument("Collision dimensions must be finite and positive");

        if (mass <= 0.0f)
            return;

        const glm::vec3 scale = glm::abs(transformScale);
        if (shape == CollisionShape::AABB)
        {
            const glm::vec3 half = dimensions * scale;
            m_Inertia = glm::vec3(
                mass * (half.y * half.y + half.z * half.z) / 3.0f,
                mass * (half.x * half.x + half.z * half.z) / 3.0f,
                mass * (half.x * half.x + half.y * half.y) / 3.0f
            );
        }
        else
        {
            const float radius = dimensions.x * std::max(scale.x, std::max(scale.y, scale.z));
            m_Inertia = glm::vec3(0.4f * mass * radius * radius);
        }
        m_InverseInertia = glm::vec3(
            m_Inertia.x > 1e-8f ? 1.0f / m_Inertia.x : 0.0f,
            m_Inertia.y > 1e-8f ? 1.0f / m_Inertia.y : 0.0f,
            m_Inertia.z > 1e-8f ? 1.0f / m_Inertia.z : 0.0f
        );
    }

    EntityId RigidBody::GetEntityId() const { return m_EntityId; }
    CollisionShape RigidBody::GetShape() const { return m_Shape; }
    float RigidBody::GetMass() const { return m_Mass; }
    const glm::vec3& RigidBody::GetDimensions() const { return m_Dimensions; }
    const glm::vec3& RigidBody::GetVelocity() const { return m_Velocity; }
    const glm::vec3& RigidBody::GetAngularVelocity() const { return m_AngularVelocity; }
    const glm::vec3& RigidBody::GetInertia() const { return m_Inertia; }
    const glm::vec3& RigidBody::GetInverseInertiaLocal() const { return m_InverseInertia; }
    float RigidBody::GetRestitution() const { return m_Restitution; }
    float RigidBody::GetFriction() const { return m_Friction; }

    void RigidBody::SetVelocity(const glm::vec3& velocity)
    {
        if (glm::any(glm::isnan(velocity)) || glm::any(glm::isinf(velocity)))
            throw std::invalid_argument("Rigid body velocity must be finite");
        m_Velocity = velocity;
    }

    void RigidBody::SetAngularVelocity(const glm::vec3& angularVelocity)
    {
        if (glm::any(glm::isnan(angularVelocity)) || glm::any(glm::isinf(angularVelocity)))
            throw std::invalid_argument("Angular velocity must be finite");
        m_AngularVelocity = m_Mass > 0.0f ? angularVelocity : glm::vec3(0.0f);
    }

    void RigidBody::SetRestitution(float restitution)
    {
        if (!std::isfinite(restitution) || restitution < 0.0f || restitution > 1.0f)
            throw std::invalid_argument("Restitution must be between 0 and 1");
        m_Restitution = restitution;
    }

    void RigidBody::SetFriction(float friction)
    {
        if (!std::isfinite(friction) || friction < 0.0f)
            throw std::invalid_argument("Friction must be finite and non-negative");
        m_Friction = friction;
    }

    void RigidBody::AddForce(const glm::vec3& force)
    {
        if (glm::any(glm::isnan(force)) || glm::any(glm::isinf(force)))
            throw std::invalid_argument("Force must be finite");
        m_Force += force;
    }

    void RigidBody::AddTorque(const glm::vec3& torque)
    {
        if (glm::any(glm::isnan(torque)) || glm::any(glm::isinf(torque)))
            throw std::invalid_argument("Torque must be finite");
        if (m_Mass > 0.0f)
            m_Torque += torque;
    }

    void RigidBody::ApplyImpulse(const glm::vec3& impulse)
    {
        if (glm::any(glm::isnan(impulse)) || glm::any(glm::isinf(impulse)))
            throw std::invalid_argument("Impulse must be finite");
        if (m_Mass > 0.0f)
            m_Velocity += impulse / m_Mass;
    }
}
