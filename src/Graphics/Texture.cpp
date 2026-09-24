#include "Graphics/Texture.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

namespace FuxEngine
{
    Texture::Texture(const std::string& path)
        : m_RendererID(0),
        m_Path(path),
        m_Width(0),
        m_Height(0),
        m_Channels(0)
    {
        // OpenGL attend l'origine (0,0) en bas à gauche de l'image,
        // mais les formats d'image (PNG, JPG...) stockent en général
        // les pixels en partant du haut. Sans ce flip, la texture
        // apparaît inversée verticalement.
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(
            path.c_str(),
            &m_Width,
            &m_Height,
            &m_Channels,
            0
        );

        if (!data)
        {
            std::cerr << "Texture: echec du chargement de " << path << std::endl;
            return;
        }

        GLenum format = GL_RGB;

        if (m_Channels == 1)
            format = GL_RED;
        else if (m_Channels == 3)
            format = GL_RGB;
        else if (m_Channels == 4)
            format = GL_RGBA;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        // Wrapping : que faire quand une coordonnee UV depasse [0, 1] ?
        // REPEAT = la texture se repete en motif (tiling).
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Filtering : comment interpoler les pixels quand la texture
        // est affichee plus petite (MIN) ou plus grande (MAG) que sa
        // resolution d'origine.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            m_Width,
            m_Height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void Texture::Bind(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void Texture::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}