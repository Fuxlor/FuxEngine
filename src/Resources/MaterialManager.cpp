#include "Resources/MaterialManager.h"

#include <fstream>
#include <stdexcept>
#include <utility>

namespace FuxEngine
{
    namespace
    {
        std::string Trim(const std::string& value)
        {
            const std::string whitespace = " \t\r\n";
            const std::size_t first = value.find_first_not_of(whitespace);
            if (first == std::string::npos)
                return {};
            const std::size_t last = value.find_last_not_of(whitespace);
            return value.substr(first, last - first + 1);
        }
    }

    MaterialManager::MaterialManager(
        ShaderManager& shaders,
        TextureManager& textures,
        std::filesystem::path materialDirectory
    )
        : m_Shaders(shaders),
        m_Textures(textures),
        m_MaterialDirectory(std::move(materialDirectory))
    {}

    Material& MaterialManager::Load(const std::string& name)
    {
        std::filesystem::path path = m_MaterialDirectory / name;
        if (path.extension().empty())
            path += ".material";

        const std::string key = std::filesystem::absolute(path).lexically_normal().generic_string();
        if (const auto found = m_Materials.find(key); found != m_Materials.end())
            return *found->second;

        std::ifstream file(path);
        if (!file)
            throw std::runtime_error("Impossible d'ouvrir le material : " + path.string());

        std::string shaderName;
        std::string textureName;
        std::string line;
        std::size_t lineNumber = 0;
        while (std::getline(file, line))
        {
            ++lineNumber;
            line = Trim(line);
            if (line.empty() || line[0] == '#' || line[0] == ';')
                continue;

            const std::size_t separator = line.find('=');
            if (separator == std::string::npos)
                throw std::runtime_error("Syntaxe invalide dans " + path.string() + ":" + std::to_string(lineNumber));

            const std::string keyName = Trim(line.substr(0, separator));
            const std::string value = Trim(line.substr(separator + 1));
            if (keyName == "shader")
                shaderName = value;
            else if (keyName == "texture")
                textureName = value;
            else
                throw std::runtime_error("Propriete inconnue '" + keyName + "' dans " + path.string());
        }

        if (shaderName.empty() || textureName.empty())
            throw std::runtime_error("Le material doit definir 'shader' et 'texture' : " + path.string());

        Shader& shader = m_Shaders.Load(shaderName);
        Texture& texture = m_Textures.Load(textureName);
        auto material = std::make_unique<Material>(shader, texture);
        Material& reference = *material;
        m_Materials.emplace(key, std::move(material));
        return reference;
    }

    std::size_t MaterialManager::GetCachedCount() const
    {
        return m_Materials.size();
    }
}
