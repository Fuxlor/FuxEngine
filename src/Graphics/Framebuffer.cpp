#include "Graphics/Framebuffer.h"

#include <stdexcept>

namespace FuxEngine
{
    Framebuffer::Framebuffer(int width, int height)
    {
        glGenFramebuffers(1, &m_RendererID);
        Resize(width, height);
    }

    Framebuffer::~Framebuffer()
    {
        if (m_DepthAttachment)
            glDeleteRenderbuffers(1, &m_DepthAttachment);
        if (m_ColorAttachment)
            glDeleteTextures(1, &m_ColorAttachment);
        if (m_RendererID)
            glDeleteFramebuffers(1, &m_RendererID);
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    }

    void Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(int width, int height)
    {
        if (width <= 0 || height <= 0)
            throw std::invalid_argument("Framebuffer dimensions must be positive");
        if (width == m_Width && height == m_Height)
            return;

        m_Width = width;
        m_Height = height;
        CreateAttachments();
    }

    void Framebuffer::CreateAttachments()
    {
        Bind();

        if (m_ColorAttachment)
            glDeleteTextures(1, &m_ColorAttachment);
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA16F,
            m_Width,
            m_Height,
            0,
            GL_RGBA,
            GL_FLOAT,
            nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            m_ColorAttachment,
            0
        );

        if (m_DepthAttachment)
            glDeleteRenderbuffers(1, &m_DepthAttachment);
        glGenRenderbuffers(1, &m_DepthAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            m_DepthAttachment
        );

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Unbind();
            throw std::runtime_error("HDR framebuffer is incomplete");
        }

        Unbind();
    }

    unsigned int Framebuffer::GetColorAttachment() const { return m_ColorAttachment; }
    int Framebuffer::GetWidth() const { return m_Width; }
    int Framebuffer::GetHeight() const { return m_Height; }
}
