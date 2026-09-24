#pragma once

#include "Core/Entity.h"

#include <memory>
#include <vector>

namespace FuxEngine
{
    class Scene
    {
    public:
        Entity& CreateEntity(
            Mesh& mesh,
            Material& material
        );

        const std::vector<std::unique_ptr<Entity>>& GetEntities() const;

    private:
        std::vector<std::unique_ptr<Entity>> m_Entities;
    };
}