#pragma once

#include "Graphics/Mesh.h"

#include <filesystem>
#include <memory>
#include <string>

namespace FuxEngine
{
    class ObjLoader
    {
    public:
        static std::unique_ptr<Mesh> Load(
            const std::string& name,
            const std::filesystem::path& modelDirectory = "assets/models"
        );
    };
}
