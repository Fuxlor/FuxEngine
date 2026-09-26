#include "Resources/ObjLoader.h"

#include <glm/glm.hpp>

#include <array>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace FuxEngine
{
    namespace
    {
        struct ObjVertexRef
        {
            int position = 0;
            int texcoord = 0;
            int normal = 0;
        };

        struct VertexKey
        {
            int position;
            int texcoord;
            int normal;
            std::size_t flatFace;

            bool operator==(const VertexKey&) const = default;
        };

        struct VertexKeyHash
        {
            std::size_t operator()(const VertexKey& key) const
            {
                std::size_t hash = std::hash<int>{}(key.position);
                hash ^= std::hash<int>{}(key.texcoord) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                hash ^= std::hash<int>{}(key.normal) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                hash ^= std::hash<std::size_t>{}(key.flatFace) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                return hash;
            }
        };

        int ParseIndexPart(std::string_view part, std::size_t lineNumber)
        {
            if (part.empty())
                return 0;

            try
            {
                std::size_t parsedLength = 0;
                const int value = std::stoi(std::string(part), &parsedLength);
                if (parsedLength != part.size() || value == 0)
                    throw std::runtime_error("index must be a non-zero integer");
                return value;
            }
            catch (const std::exception&)
            {
                throw std::runtime_error("Index OBJ invalide a la ligne " + std::to_string(lineNumber));
            }
        }

        int ResolveIndex(int index, std::size_t count, std::size_t lineNumber)
        {
            const long long resolved = index > 0
                ? static_cast<long long>(index) - 1
                : static_cast<long long>(count) + index;
            if (resolved < 0 || resolved >= static_cast<long long>(count))
                throw std::runtime_error("Index OBJ hors limites a la ligne " + std::to_string(lineNumber));
            return static_cast<int>(resolved);
        }

        int ResolveOptionalIndex(int index, std::size_t count, std::size_t lineNumber)
        {
            return index == 0 ? -1 : ResolveIndex(index, count, lineNumber);
        }

        ObjVertexRef ParseVertexRef(
            const std::string& token,
            std::size_t positionCount,
            std::size_t texcoordCount,
            std::size_t normalCount,
            std::size_t lineNumber
        )
        {
            const std::size_t firstSlash = token.find('/');
            const std::size_t secondSlash = firstSlash == std::string::npos
                ? std::string::npos
                : token.find('/', firstSlash + 1);

            const std::string_view positionPart(token.data(),
                firstSlash == std::string::npos ? token.size() : firstSlash);
            const std::string_view texcoordPart = firstSlash == std::string::npos
                ? std::string_view{}
                : std::string_view(token).substr(firstSlash + 1,
                    secondSlash == std::string::npos ? std::string::npos : secondSlash - firstSlash - 1);
            const std::string_view normalPart = secondSlash == std::string::npos
                ? std::string_view{}
                : std::string_view(token).substr(secondSlash + 1);

            if (secondSlash != std::string::npos && normalPart.find('/') != std::string::npos)
                throw std::runtime_error("Face OBJ invalide a la ligne " + std::to_string(lineNumber));

            const int rawPosition = ParseIndexPart(positionPart, lineNumber);
            const int rawTexcoord = ParseIndexPart(texcoordPart, lineNumber);
            const int rawNormal = ParseIndexPart(normalPart, lineNumber);
            if (rawPosition == 0)
                throw std::runtime_error("Position OBJ manquante a la ligne " + std::to_string(lineNumber));

            return {
                ResolveIndex(rawPosition, positionCount, lineNumber),
                ResolveOptionalIndex(rawTexcoord, texcoordCount, lineNumber),
                ResolveOptionalIndex(rawNormal, normalCount, lineNumber)
            };
        }
    }

    std::unique_ptr<Mesh> ObjLoader::Load(
        const std::string& name,
        const std::filesystem::path& modelDirectory
    )
    {
        const std::filesystem::path path = modelDirectory / name;
        std::ifstream file(path);
        if (!file)
            throw std::runtime_error("Impossible d'ouvrir le modele OBJ : " + path.string());

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> texcoords;
        std::vector<glm::vec3> normals;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::unordered_map<VertexKey, unsigned int, VertexKeyHash> vertexLookup;
        std::string line;
        std::size_t lineNumber = 0;
        std::size_t faceNumber = 0;

        while (std::getline(file, line))
        {
            ++lineNumber;
            const std::size_t comment = line.find('#');
            if (comment != std::string::npos)
                line.erase(comment);

            std::istringstream stream(line);
            std::string command;
            stream >> command;

            if (command == "v")
            {
                glm::vec3 position;
                if (!(stream >> position.x >> position.y >> position.z))
                    throw std::runtime_error("Vertex OBJ invalide a la ligne " + std::to_string(lineNumber));
                positions.push_back(position);
            }
            else if (command == "vt")
            {
                glm::vec2 texcoord;
                if (!(stream >> texcoord.x >> texcoord.y))
                    throw std::runtime_error("Coordonnee UV OBJ invalide a la ligne " + std::to_string(lineNumber));
                texcoords.push_back(texcoord);
            }
            else if (command == "vn")
            {
                glm::vec3 normal;
                if (!(stream >> normal.x >> normal.y >> normal.z))
                    throw std::runtime_error("Normale OBJ invalide a la ligne " + std::to_string(lineNumber));
                normals.push_back(normal);
            }
            else if (command == "f")
            {
                std::vector<ObjVertexRef> face;
                std::string token;
                while (stream >> token)
                {
                    face.push_back(ParseVertexRef(
                        token, positions.size(), texcoords.size(), normals.size(), lineNumber));
                }
                if (face.size() < 3)
                    throw std::runtime_error("Face OBJ avec moins de trois sommets a la ligne " + std::to_string(lineNumber));

                const glm::vec3 edge1 = positions[face[1].position] - positions[face[0].position];
                const glm::vec3 edge2 = positions[face[2].position] - positions[face[0].position];
                const glm::vec3 cross = glm::cross(edge1, edge2);
                const glm::vec3 flatNormal = glm::length(cross) > 0.0f
                    ? glm::normalize(cross)
                    : glm::vec3(0.0f, 1.0f, 0.0f);
                ++faceNumber;

                auto appendVertex = [&](const ObjVertexRef& ref)
                {
                    const VertexKey key{
                        ref.position,
                        ref.texcoord,
                        ref.normal,
                        ref.normal < 0 ? faceNumber : 0
                    };

                    if (const auto found = vertexLookup.find(key); found != vertexLookup.end())
                    {
                        indices.push_back(found->second);
                        return;
                    }

                    if (vertices.size() / 11 >= std::numeric_limits<unsigned int>::max())
                        throw std::runtime_error("Le modele OBJ contient trop de sommets : " + path.string());

                    const glm::vec3& position = positions[ref.position];
                    const glm::vec3 sourceNormal = ref.normal < 0
                        ? flatNormal
                        : normals[ref.normal];
                    const glm::vec3 normal = glm::length(sourceNormal) > 0.0f
                        ? glm::normalize(sourceNormal)
                        : flatNormal;
                    const glm::vec2 texcoord = ref.texcoord < 0
                        ? glm::vec2(0.0f)
                        : texcoords[ref.texcoord];

                    const unsigned int index = static_cast<unsigned int>(vertices.size() / 11);
                    vertices.insert(vertices.end(), {
                        position.x, position.y, position.z,
                        normal.x, normal.y, normal.z,
                        1.0f, 1.0f, 1.0f,
                        texcoord.x, texcoord.y
                    });
                    vertexLookup.emplace(key, index);
                    indices.push_back(index);
                };

                for (std::size_t corner = 1; corner + 1 < face.size(); ++corner)
                {
                    appendVertex(face[0]);
                    appendVertex(face[corner]);
                    appendVertex(face[corner + 1]);
                }
            }
        }

        if (indices.empty())
            throw std::runtime_error("Le modele OBJ ne contient aucune face : " + path.string());

        if (vertices.size() * sizeof(float) > std::numeric_limits<unsigned int>::max() ||
            indices.size() > std::numeric_limits<unsigned int>::max())
            throw std::runtime_error("Le modele OBJ est trop volumineux : " + path.string());

        return std::make_unique<Mesh>(
            vertices.data(),
            static_cast<unsigned int>(vertices.size() * sizeof(float)),
            indices.data(),
            static_cast<unsigned int>(indices.size())
        );
    }
}
