#include "Resources/TextureManager.h"

#include <utility>

namespace FuxEngine
{
    TextureManager::TextureManager(std::filesystem::path textureDirectory)
        : m_TextureDirectory(std::move(textureDirectory))
    {}

    Texture& TextureManager::Load(const std::string& name, bool colorTexture)
    {
        const std::filesystem::path path = m_TextureDirectory / name;
        const std::string key = std::filesystem::absolute(path).lexically_normal().generic_string() +
            (colorTexture ? "|srgb" : "|linear");

        if (const auto found = m_Textures.find(key); found != m_Textures.end())
            return *found->second;

        auto texture = std::make_unique<Texture>(path.string(), colorTexture);
        Texture& reference = *texture;
        m_Textures.emplace(key, std::move(texture));
        return reference;
    }

    std::size_t TextureManager::GetCachedCount() const
    {
        return m_Textures.size();
    }
}
