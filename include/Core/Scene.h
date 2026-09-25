#pragma once

#include "Core/Entity.h"
#include "Core/Light.h"

#include <vector>
#include <memory>
#include <utility>

namespace FuxEngine
{
    class Scene
    {
    public:
        Entity& CreateEntity(
            Mesh& mesh,
            Material& material
        );

        template<typename T, typename... Args>
        T& CreateLight(Args&&... args)
        {
            auto light = std::make_unique<T>(
                std::forward<Args>(args)...
            );

            T& reference = *light;

            m_Lights.push_back(std::move(light));

            return reference;
        }

        const std::vector<std::unique_ptr<Entity>>& GetEntities() const;
        const std::vector<std::unique_ptr<Light>>&
            GetLights() const
        {
            return m_Lights;
        }

    private:
        std::vector<std::unique_ptr<Entity>> m_Entities;
        std::vector<std::unique_ptr<Light>> m_Lights;
    };
}