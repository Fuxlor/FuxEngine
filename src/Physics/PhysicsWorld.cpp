#include "Physics/PhysicsWorld.h"
#include "Core/Scene.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>

namespace FuxEngine
{
    namespace
    {
        struct Contact
        {
            glm::vec3 point;
            glm::vec3 normal;
            float penetration;
        };

        float InverseMass(const RigidBody& body)
        {
            return body.GetMass() > 0.0f ? 1.0f / body.GetMass() : 0.0f;
        }

        glm::mat3 WorldInverseInertia(const Entity& entity, const RigidBody& body)
        {
            const glm::vec3 rotationDegrees = entity.GetTransform().GetRotation();
            glm::mat4 rotation(1.0f);
            rotation = glm::rotate(rotation, glm::radians(rotationDegrees.x), glm::vec3(1, 0, 0));
            rotation = glm::rotate(rotation, glm::radians(rotationDegrees.y), glm::vec3(0, 1, 0));
            rotation = glm::rotate(rotation, glm::radians(rotationDegrees.z), glm::vec3(0, 0, 1));
            const glm::mat3 basis(rotation);
            const glm::vec3 inverse = body.GetInverseInertiaLocal();
            const glm::mat3 localInverse(
                glm::vec3(inverse.x, 0, 0),
                glm::vec3(0, inverse.y, 0),
                glm::vec3(0, 0, inverse.z)
            );
            return basis * localInverse * glm::transpose(basis);
        }

        float EffectiveMass(const Entity& entityA, const RigidBody& bodyA, const glm::vec3& offsetA,
                            const Entity& entityB, const RigidBody& bodyB, const glm::vec3& offsetB,
                            const glm::vec3& axis)
        {
            const glm::vec3 angularA = glm::cross(
                WorldInverseInertia(entityA, bodyA) * glm::cross(offsetA, axis), offsetA);
            const glm::vec3 angularB = glm::cross(
                WorldInverseInertia(entityB, bodyB) * glm::cross(offsetB, axis), offsetB);
            return InverseMass(bodyA) + InverseMass(bodyB) +
                glm::dot(axis, angularA + angularB);
        }

        bool Collide(const Entity& a, const RigidBody& bodyA,
                     const Entity& b, const RigidBody& bodyB, Contact& contact)
        {
            const glm::vec3 positionA = a.GetTransform().GetPosition();
            const glm::vec3 positionB = b.GetTransform().GetPosition();
            const glm::vec3 delta = positionB - positionA;

            if (bodyA.GetShape() == CollisionShape::Sphere && bodyB.GetShape() == CollisionShape::Sphere)
            {
                // Sphere dimensions use x as the local radius; entity scale is applied below.
                const glm::vec3 scaleA = glm::abs(a.GetTransform().GetScale());
                const glm::vec3 scaleB = glm::abs(b.GetTransform().GetScale());
                const float radiusA = bodyA.GetDimensions().x * std::max(scaleA.x, std::max(scaleA.y, scaleA.z));
                const float radiusB = bodyB.GetDimensions().x * std::max(scaleB.x, std::max(scaleB.y, scaleB.z));
                const float distanceSquared = glm::dot(delta, delta);
                const float radii = radiusA + radiusB;
                if (distanceSquared >= radii * radii)
                    return false;
                const float distance = std::sqrt(distanceSquared);
                contact.normal = distance > 1e-6f ? delta / distance : glm::vec3(1.0f, 0.0f, 0.0f);
                contact.penetration = radii - distance;
                contact.point = positionA + contact.normal * (radiusA - contact.penetration * 0.5f);
                return true;
            }

            const bool aIsSphere = bodyA.GetShape() == CollisionShape::Sphere;
            const bool bIsSphere = bodyB.GetShape() == CollisionShape::Sphere;
            if (aIsSphere != bIsSphere)
            {
                const Entity& sphereEntity = aIsSphere ? a : b;
                const Entity& boxEntity = aIsSphere ? b : a;
                const RigidBody& sphereBody = aIsSphere ? bodyA : bodyB;
                const RigidBody& boxBody = aIsSphere ? bodyB : bodyA;
                const glm::vec3 sphereCenter = sphereEntity.GetTransform().GetPosition();
                const glm::vec3 boxCenter = boxEntity.GetTransform().GetPosition();
                const glm::vec3 halfExtents = boxBody.GetDimensions() *
                    glm::abs(boxEntity.GetTransform().GetScale());
                const glm::vec3 local = sphereCenter - boxCenter;
                const glm::vec3 closestPoint = boxCenter + glm::clamp(local, -halfExtents, halfExtents);
                glm::vec3 sphereToBox = closestPoint - sphereCenter;
                const float distanceSquared = glm::dot(sphereToBox, sphereToBox);
                const glm::vec3 scale = glm::abs(sphereEntity.GetTransform().GetScale());
                const float radius = sphereBody.GetDimensions().x * std::max(scale.x, std::max(scale.y, scale.z));

                glm::vec3 sphereToBoxNormal;
                if (distanceSquared > 1e-12f)
                {
                    const float distance = std::sqrt(distanceSquared);
                    if (distance >= radius)
                        return false;
                    sphereToBoxNormal = sphereToBox / distance;
                    contact.penetration = radius - distance;
                }
                else
                {
                    // The sphere center is inside the box: expel it through the nearest face.
                    const glm::vec3 faceDistances = halfExtents - glm::abs(local);
                    glm::vec3 outward(0.0f);
                    float distanceToFace;
                    if (faceDistances.x <= faceDistances.y && faceDistances.x <= faceDistances.z)
                    {
                        distanceToFace = faceDistances.x;
                        outward.x = local.x < 0.0f ? -1.0f : 1.0f;
                    }
                    else if (faceDistances.y <= faceDistances.z)
                    {
                        distanceToFace = faceDistances.y;
                        outward.y = local.y < 0.0f ? -1.0f : 1.0f;
                    }
                    else
                    {
                        distanceToFace = faceDistances.z;
                        outward.z = local.z < 0.0f ? -1.0f : 1.0f;
                    }
                    sphereToBoxNormal = -outward;
                    contact.penetration = radius + distanceToFace;
                    contact.point = sphereCenter + outward * distanceToFace;
                }

                contact.normal = aIsSphere ? sphereToBoxNormal : -sphereToBoxNormal;
                if (distanceSquared > 1e-12f)
                    contact.point = closestPoint;
                return true;
            }

            if (bodyA.GetShape() != CollisionShape::AABB || bodyB.GetShape() != CollisionShape::AABB)
                return false;

            const glm::vec3 halfA = bodyA.GetDimensions() * glm::abs(a.GetTransform().GetScale());
            const glm::vec3 halfB = bodyB.GetDimensions() * glm::abs(b.GetTransform().GetScale());
            const glm::vec3 overlap = halfA + halfB - glm::abs(delta);
            if (glm::any(glm::lessThanEqual(overlap, glm::vec3(0.0f))))
                return false;

            if (overlap.x <= overlap.y && overlap.x <= overlap.z)
                contact.normal = glm::vec3(delta.x < 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
            else if (overlap.y <= overlap.z)
                contact.normal = glm::vec3(0.0f, delta.y < 0.0f ? -1.0f : 1.0f, 0.0f);
            else
                contact.normal = glm::vec3(0.0f, 0.0f, delta.z < 0.0f ? -1.0f : 1.0f);
            contact.penetration = std::min(overlap.x, std::min(overlap.y, overlap.z));
            const glm::vec3 minCorner = glm::max(positionA - halfA, positionB - halfB);
            const glm::vec3 maxCorner = glm::min(positionA + halfA, positionB + halfB);
            contact.point = (minCorner + maxCorner) * 0.5f;
            return true;
        }

        void Resolve(Entity& a, RigidBody& bodyA, Entity& b, RigidBody& bodyB, const Contact& contact)
        {
            const float inverseMassA = InverseMass(bodyA);
            const float inverseMassB = InverseMass(bodyB);
            const float inverseMassSum = inverseMassA + inverseMassB;
            if (inverseMassSum <= 0.0f)
                return;

            // Positional correction prevents small integration errors from accumulating.
            constexpr float slop = 0.001f;
            constexpr float correctionPercent = 0.8f;
            const glm::vec3 correction = contact.normal *
                (std::max(contact.penetration - slop, 0.0f) * correctionPercent / inverseMassSum);
            if (inverseMassA > 0.0f)
                a.GetTransform().Translate(-correction * inverseMassA);
            if (inverseMassB > 0.0f)
                b.GetTransform().Translate(correction * inverseMassB);

            const glm::vec3 offsetA = contact.point - a.GetTransform().GetPosition();
            const glm::vec3 offsetB = contact.point - b.GetTransform().GetPosition();
            glm::vec3 velocityA = bodyA.GetVelocity();
            glm::vec3 velocityB = bodyB.GetVelocity();
            glm::vec3 angularVelocityA = bodyA.GetAngularVelocity();
            glm::vec3 angularVelocityB = bodyB.GetAngularVelocity();
            glm::vec3 relativeVelocity =
                (velocityB + glm::cross(angularVelocityB, offsetB)) -
                (velocityA + glm::cross(angularVelocityA, offsetA));
            const float velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);
            if (velocityAlongNormal > 0.0f)
                return;

            const float restitution = std::min(bodyA.GetRestitution(), bodyB.GetRestitution());
            const float normalMass = EffectiveMass(a, bodyA, offsetA, b, bodyB, offsetB, contact.normal);
            if (normalMass <= 1e-8f)
                return;
            const float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / normalMass;
            const glm::vec3 impulse = impulseMagnitude * contact.normal;
            if (inverseMassA > 0.0f)
            {
                velocityA -= impulse * inverseMassA;
                angularVelocityA -= WorldInverseInertia(a, bodyA) * glm::cross(offsetA, impulse);
            }
            if (inverseMassB > 0.0f)
            {
                velocityB += impulse * inverseMassB;
                angularVelocityB += WorldInverseInertia(b, bodyB) * glm::cross(offsetB, impulse);
            }

            bodyA.SetVelocity(velocityA);
            bodyB.SetVelocity(velocityB);
            bodyA.SetAngularVelocity(angularVelocityA);
            bodyB.SetAngularVelocity(angularVelocityB);

            relativeVelocity =
                (velocityB + glm::cross(angularVelocityB, offsetB)) -
                (velocityA + glm::cross(angularVelocityA, offsetA));
            glm::vec3 tangent = relativeVelocity - glm::dot(relativeVelocity, contact.normal) * contact.normal;
            const float tangentLength = glm::length(tangent);
            if (tangentLength <= 1e-6f)
                return;
            tangent /= tangentLength;
            const float tangentMass = EffectiveMass(a, bodyA, offsetA, b, bodyB, offsetB, tangent);
            if (tangentMass <= 1e-8f)
                return;
            float frictionMagnitude = -glm::dot(relativeVelocity, tangent) / tangentMass;
            const float friction = std::sqrt(bodyA.GetFriction() * bodyB.GetFriction());
            frictionMagnitude = glm::clamp(frictionMagnitude,
                -impulseMagnitude * friction, impulseMagnitude * friction);
            const glm::vec3 frictionImpulse = frictionMagnitude * tangent;
            if (inverseMassA > 0.0f)
            {
                velocityA -= frictionImpulse * inverseMassA;
                angularVelocityA -= WorldInverseInertia(a, bodyA) * glm::cross(offsetA, frictionImpulse);
            }
            if (inverseMassB > 0.0f)
            {
                velocityB += frictionImpulse * inverseMassB;
                angularVelocityB += WorldInverseInertia(b, bodyB) * glm::cross(offsetB, frictionImpulse);
            }
            bodyA.SetVelocity(velocityA);
            bodyB.SetVelocity(velocityB);
            bodyA.SetAngularVelocity(angularVelocityA);
            bodyB.SetAngularVelocity(angularVelocityB);
        }
    }

    PhysicsWorld::PhysicsWorld(glm::vec3 gravity) : m_Gravity(gravity)
    {
        SetGravity(gravity);
    }

    RigidBody& PhysicsWorld::CreateBody(Entity& entity, CollisionShape shape, float mass,
                                        const glm::vec3& dimensions)
    {
        if (FindBody(entity.GetId()))
            throw std::invalid_argument("Entity already has a rigid body");
        m_Bodies.push_back(std::unique_ptr<RigidBody>(new RigidBody(
            entity.GetId(), shape, mass, dimensions, entity.GetTransform().GetScale())));
        return *m_Bodies.back();
    }

    bool PhysicsWorld::RemoveBody(EntityId entityId)
    {
        const auto body = std::find_if(m_Bodies.begin(), m_Bodies.end(),
            [entityId](const auto& candidate) { return candidate->GetEntityId() == entityId; });
        if (body == m_Bodies.end())
            return false;
        m_Bodies.erase(body);
        return true;
    }

    RigidBody* PhysicsWorld::FindBody(EntityId entityId)
    {
        for (const auto& body : m_Bodies)
            if (body->GetEntityId() == entityId)
                return body.get();
        return nullptr;
    }

    const std::vector<std::unique_ptr<RigidBody>>& PhysicsWorld::GetBodies() const { return m_Bodies; }
    const std::vector<PhysicsContact>& PhysicsWorld::GetLastContacts() const { return m_LastContacts; }

    void PhysicsWorld::SetGravity(const glm::vec3& gravity)
    {
        if (glm::any(glm::isnan(gravity)) || glm::any(glm::isinf(gravity)))
            throw std::invalid_argument("Gravity must be finite");
        m_Gravity = gravity;
    }

    const glm::vec3& PhysicsWorld::GetGravity() const { return m_Gravity; }

    void PhysicsWorld::Step(Scene& scene, float deltaTime)
    {
        if (!std::isfinite(deltaTime) || deltaTime < 0.0f)
            throw std::invalid_argument("Physics delta time must be finite and non-negative");
        if (deltaTime == 0.0f)
            return;

        m_LastContacts.clear();
        std::erase_if(m_Bodies, [&scene](const auto& body) { return scene.FindEntity(body->GetEntityId()) == nullptr; });
        for (auto& body : m_Bodies)
        {
            if (body->m_Mass <= 0.0f)
            {
                body->m_Force = glm::vec3(0.0f);
                continue;
            }
            Entity* entity = scene.FindEntity(body->m_EntityId);
            const glm::vec3 acceleration = m_Gravity + body->m_Force / body->m_Mass;
            body->m_Velocity += acceleration * deltaTime;
            entity->GetTransform().Translate(body->m_Velocity * deltaTime);
            const glm::mat3 inverseInertia = WorldInverseInertia(*entity, *body);
            body->m_AngularVelocity += inverseInertia * body->m_Torque * deltaTime;
            entity->GetTransform().Rotate(glm::degrees(body->m_AngularVelocity * deltaTime));
            body->m_Force = glm::vec3(0.0f);
            body->m_Torque = glm::vec3(0.0f);
        }

        constexpr int solverIterations = 8;
        for (int iteration = 0; iteration < solverIterations; ++iteration)
        {
            for (std::size_t i = 0; i < m_Bodies.size(); ++i)
            {
                for (std::size_t j = i + 1; j < m_Bodies.size(); ++j)
                {
                    RigidBody& bodyA = *m_Bodies[i];
                    RigidBody& bodyB = *m_Bodies[j];
                    if (bodyA.m_Mass <= 0.0f && bodyB.m_Mass <= 0.0f)
                        continue;
                    Entity* entityA = scene.FindEntity(bodyA.m_EntityId);
                    Entity* entityB = scene.FindEntity(bodyB.m_EntityId);
                    Contact contact;
                    if (Collide(*entityA, bodyA, *entityB, bodyB, contact))
                    {
                        if (iteration == 0)
                        {
                            m_LastContacts.push_back({contact.point, contact.normal});
                        }
                        Resolve(*entityA, bodyA, *entityB, bodyB, contact);
                    }
                }
            }
        }
    }
}
