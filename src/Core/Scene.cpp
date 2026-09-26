#include "Core/Scene.h"

#include <algorithm>
#include <stdexcept>

namespace FuxEngine
{
    Entity& Scene::CreateEntity(
        Mesh& mesh,
        Material& material,
        std::string name
    )
    {
        if (m_NextEntityId == 0)
            throw std::overflow_error("Scene entity ID space is exhausted");

        const EntityId id = m_NextEntityId;
        if (name.empty())
            name = "Entity_" + std::to_string(id);

        m_Entities.push_back(
            std::make_unique<Entity>(
                id,
                std::move(name),
                mesh,
                material
            )
        );
        ++m_NextEntityId;

        return *m_Entities.back();
    }

    bool Scene::RemoveEntity(EntityId id)
    {
        const auto entity = std::find_if(
            m_Entities.begin(),
            m_Entities.end(),
            [id](const std::unique_ptr<Entity>& candidate)
            {
                return candidate->GetId() == id;
            }
        );

        if (entity == m_Entities.end())
            return false;

        m_Entities.erase(entity);
        return true;
    }

    Entity* Scene::FindEntity(EntityId id)
    {
        for (const auto& entity : m_Entities)
        {
            if (entity->GetId() == id)
                return entity.get();
        }
        return nullptr;
    }

    const Entity* Scene::FindEntity(EntityId id) const
    {
        for (const auto& entity : m_Entities)
        {
            if (entity->GetId() == id)
                return entity.get();
        }
        return nullptr;
    }

    Entity* Scene::FindEntityByName(const std::string& name)
    {
        for (const auto& entity : m_Entities)
        {
            if (entity->GetName() == name)
                return entity.get();
        }
        return nullptr;
    }

    const Entity* Scene::FindEntityByName(const std::string& name) const
    {
        for (const auto& entity : m_Entities)
        {
            if (entity->GetName() == name)
                return entity.get();
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Entity>>&
        Scene::GetEntities() const
    {
        return m_Entities;
    }

    const std::vector<std::unique_ptr<Light>>&
    Scene::GetLights() const
    {
        return m_Lights;
    }

    const std::vector<const PointLight*>& Scene::GetPointLights() const
    {
        return m_PointLights;
    }

    const std::vector<const DirectionalLight*>& Scene::GetDirectionalLights() const
    {
        return m_DirectionalLights;
    }

    const std::vector<const SpotLight*>& Scene::GetSpotLights() const
    {
        return m_SpotLights;
    }
}
