#pragma once

#include "Core/Entity.h"
#include "Lighting/Light.h"
#include "Lighting/PointLight.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/SpotLight.h"

#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <type_traits>

namespace FuxEngine
{
    class Scene
    {
    public:
        // Scene owns the returned entity; its reference ends at removal or scene destruction.
        Entity& CreateEntity(
            Mesh& mesh,
            Material& material,
            std::string name = {}
        );

        bool RemoveEntity(EntityId id);
        // Returned pointers are non-owning and invalidated when the entity is removed.
        Entity* FindEntity(EntityId id);
        const Entity* FindEntity(EntityId id) const;
        Entity* FindEntityByName(const std::string& name);
        const Entity* FindEntityByName(const std::string& name) const;

        template<typename T, typename... Args>
        T& CreateLight(Args&&... args)
        {
            auto light = std::make_unique<T>(
                std::forward<Args>(args)...
            );

            T& reference = *light;

            m_Lights.push_back(std::move(light));

            if constexpr (std::is_same_v<T, PointLight>)
                m_PointLights.push_back(&reference);
            else if constexpr (std::is_same_v<T, DirectionalLight>)
                m_DirectionalLights.push_back(&reference);
            else if constexpr (std::is_same_v<T, SpotLight>)
                m_SpotLights.push_back(&reference);
            else
                static_assert(
                    std::is_same_v<T, PointLight> ||
                    std::is_same_v<T, DirectionalLight> ||
                    std::is_same_v<T, SpotLight>,
                    "Scene::CreateLight only supports built-in light types"
                );

            return reference;
        }

        const std::vector<std::unique_ptr<Entity>>& GetEntities() const;
        const std::vector<std::unique_ptr<Light>>& GetLights() const;
        const std::vector<const PointLight*>& GetPointLights() const;
        const std::vector<const DirectionalLight*>& GetDirectionalLights() const;
        const std::vector<const SpotLight*>& GetSpotLights() const;

    private:
        std::vector<std::unique_ptr<Entity>> m_Entities;
        EntityId m_NextEntityId = 1;
        std::vector<std::unique_ptr<Light>> m_Lights;
        std::vector<const PointLight*> m_PointLights;
        std::vector<const DirectionalLight*> m_DirectionalLights;
        std::vector<const SpotLight*> m_SpotLights;
    };
}
