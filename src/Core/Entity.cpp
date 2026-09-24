#include "Core/Entity.h"

namespace FuxEngine
{
    Entity::Entity(
        Mesh& mesh,
        Material& material
    )
        : m_Mesh(mesh),
        m_Material(material)
    {}

    Transform& Entity::GetTransform()
    {
        return m_Transform;
    }

    const Transform& Entity::GetTransform() const
    {
        return m_Transform;
    }

    Mesh& Entity::GetMesh()
    {
        return m_Mesh;
    }

    const Mesh& Entity::GetMesh() const
    {
        return m_Mesh;
    }

    Material& Entity::GetMaterial()
    {
        return m_Material;
    }

    const Material& Entity::GetMaterial() const
    {
        return m_Material;
    }
}