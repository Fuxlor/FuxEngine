#pragma once

#include "Graphics/Material.h"
#include "Resources/ShaderManager.h"
#include "Resources/TextureManager.h"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace FuxEngine
{
    class MaterialManager
    {
    public:
        MaterialManager(
            ShaderManager& shaders,
            TextureManager& textures,
            std::filesystem::path materialDirectory = "assets/materials"
        );

        Material& Load(const std::string& name);
        std::size_t GetCachedCount() const;

    private:
        ShaderManager& m_Shaders;
        TextureManager& m_Textures;
        std::filesystem::path m_MaterialDirectory;
        std::unordered_map<std::string, std::unique_ptr<Material>> m_Materials;
    };
}
