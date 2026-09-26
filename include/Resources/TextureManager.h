#pragma once

#include "Graphics/Texture.h"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace FuxEngine
{
    class TextureManager
    {
    public:
        explicit TextureManager(
            std::filesystem::path textureDirectory = "assets/textures"
        );

        Texture& Load(const std::string& name, bool colorTexture = false);
        std::size_t GetCachedCount() const;

    private:
        std::filesystem::path m_TextureDirectory;
        std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
    };
}
