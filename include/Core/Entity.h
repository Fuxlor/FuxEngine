#pragma once

#include "Graphics/Mesh.h"
#include "Graphics/Transform.h"
#include "Graphics/Material.h"

namespace FuxEngine
{
    class Entity
    {
        public:
            Entity(
                Mesh& mesh,
                Material& material
            );

            Transform& GetTransform();
            const Transform& GetTransform() const;

            Mesh& GetMesh();
            const Mesh& GetMesh() const;

            Material& GetMaterial();
            const Material& GetMaterial() const;

        private:
            Mesh& m_Mesh;
            Material& m_Material;
            Transform m_Transform;
    };
}