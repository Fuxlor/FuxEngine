#pragma once

#include "Graphics/Shader.h"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace FuxEngine
{
    class ShaderManager
    {
    public:
        explicit ShaderManager(
            std::filesystem::path shaderDirectory = "assets/shaders"
        );

        Shader& Load(const std::string& name);
        std::size_t GetCachedCount() const;

    private:
        std::filesystem::path m_ShaderDirectory;
        std::unordered_map<std::string, std::unique_ptr<Shader>> m_Shaders;
    };
}
