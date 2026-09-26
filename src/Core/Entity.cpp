#include "Core/Entity.h"

#include <utility>

namespace FuxEngine
{
    Entity::Entity(
        EntityId id,
        std::string name,
        Mesh& mesh,
        Material& material
    )
        : m_Id(id),
        m_Name(std::move(name)),
        m_Mesh(mesh),
        m_Material(material)
    {}

    EntityId Entity::GetId() const
    {
        return m_Id;
    }

    const std::string& Entity::GetName() const
    {
        return m_Name;
    }

    void Entity::SetName(std::string name)
    {
        m_Name = std::move(name);
    }

    bool Entity::IsEnabled() const
    {
        return m_Enabled;
    }

    void Entity::SetEnabled(bool enabled)
    {
        m_Enabled = enabled;
    }

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
