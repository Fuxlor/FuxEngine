#pragma once

#include <string>

namespace FuxEngine
{
    class Texture
    {
        public:
            Texture(const std::string& path);
            ~Texture();

            void Bind(unsigned int slot = 0) const;
            void Unbind() const;

            int GetWidth() const { return m_Width; }
            int GetHeight() const { return m_Height; }

        private:
            unsigned int m_RendererID;
            std::string m_Path;

            int m_Width;
            int m_Height;
            int m_Channels;
    };
}