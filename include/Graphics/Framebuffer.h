#pragma once

#include <glad/glad.h>

namespace FuxEngine
{
    class Framebuffer
    {
    public:
        Framebuffer(int width, int height);
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        void Bind() const;
        static void Unbind();
        void Resize(int width, int height);

        unsigned int GetColorAttachment() const;
        int GetWidth() const;
        int GetHeight() const;

    private:
        void CreateAttachments();

        unsigned int m_RendererID = 0;
        unsigned int m_ColorAttachment = 0;
        unsigned int m_DepthAttachment = 0;
        int m_Width = 0;
        int m_Height = 0;
    };
}
