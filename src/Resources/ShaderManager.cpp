#include "Resources/ShaderManager.h"

#include <filesystem>
#include <utility>

namespace FuxEngine
{
    ShaderManager::ShaderManager(std::filesystem::path shaderDirectory)
        : m_ShaderDirectory(std::move(shaderDirectory))
    {}

    Shader& ShaderManager::Load(const std::string& name)
    {
        const std::filesystem::path vertexPath = m_ShaderDirectory / (name + ".vert.glsl");
        const std::filesystem::path fragmentPath = m_ShaderDirectory / (name + ".frag.glsl");
        const std::string key = std::filesystem::absolute(vertexPath).lexically_normal().generic_string() +
            "|" + std::filesystem::absolute(fragmentPath).lexically_normal().generic_string();

        if (const auto found = m_Shaders.find(key); found != m_Shaders.end())
            return *found->second;

        auto shader = std::make_unique<Shader>(vertexPath.string(), fragmentPath.string());
        Shader& reference = *shader;
        m_Shaders.emplace(key, std::move(shader));
        return reference;
    }

    std::size_t ShaderManager::GetCachedCount() const
    {
        return m_Shaders.size();
    }
}
