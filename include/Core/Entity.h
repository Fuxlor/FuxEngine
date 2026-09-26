#pragma once

#include "Graphics/Mesh.h"
#include "Graphics/Transform.h"
#include "Graphics/Material.h"

#include <cstdint>
#include <string>

namespace FuxEngine
{
    using EntityId = std::uint64_t;

    class Entity
    {
        public:
            Entity(
                EntityId id,
                std::string name,
                Mesh& mesh,
                Material& material
            );

            EntityId GetId() const;
            const std::string& GetName() const;
            void SetName(std::string name);

            bool IsEnabled() const;
            void SetEnabled(bool enabled);

            Transform& GetTransform();
            const Transform& GetTransform() const;

            Mesh& GetMesh();
            const Mesh& GetMesh() const;

            Material& GetMaterial();
            const Material& GetMaterial() const;

        private:
            EntityId m_Id;
            std::string m_Name;
            bool m_Enabled = true;
            Mesh& m_Mesh;
            Material& m_Material;
            Transform m_Transform;
    };
}
