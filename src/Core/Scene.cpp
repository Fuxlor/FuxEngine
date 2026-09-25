#include "Core/Scene.h"

namespace FuxEngine
{
    Entity& Scene::CreateEntity(
        Mesh& mesh,
        Material& material
    )
    {
        m_Entities.push_back(
            std::make_unique<Entity>(
                mesh,
                material
            )
        );

        return *m_Entities.back();
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
}