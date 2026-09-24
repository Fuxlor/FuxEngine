#include "Core/Scene.h"

#include <memory>

namespace FuxEngine
{
    Entity& Scene::CreateEntity(
        Mesh& mesh,
        Material& material
    )
    {
        auto entity = std::make_unique<Entity>(
            mesh,
            material
        );

        Entity& reference = *entity;

        m_Entities.push_back(std::move(entity));

        return reference;
    }

    const std::vector<std::unique_ptr<Entity>>& Scene::GetEntities() const
    {
        return m_Entities;
    }
}