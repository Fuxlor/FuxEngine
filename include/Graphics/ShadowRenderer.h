#pragma once

#include "Graphics/Shader.h"

namespace FuxEngine
{
    class Scene;

    class ShadowRenderer
    {
    public:
        explicit ShadowRenderer(Shader& depthShader, int resolution = 2048);
        ~ShadowRenderer();

        ShadowRenderer(const ShadowRenderer&) = delete;
        ShadowRenderer& operator=(const ShadowRenderer&) = delete;

        bool Render(const Scene& scene);
        bool HasShadowMap() const;
        const glm::mat4& GetLightSpaceMatrix() const;
        void BindTexture(unsigned int slot) const;

    private:
        Shader& m_DepthShader;
        unsigned int m_Framebuffer = 0;
        unsigned int m_DepthTexture = 0;
        int m_Resolution;
        bool m_HasShadowMap = false;
        glm::mat4 m_LightSpaceMatrix{1.0f};
    };
}
