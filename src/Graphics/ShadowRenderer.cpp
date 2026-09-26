#include "Graphics/ShadowRenderer.h"

#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Lighting/DirectionalLight.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

namespace FuxEngine
{
    ShadowRenderer::ShadowRenderer(Shader& depthShader, int resolution)
        : m_DepthShader(depthShader),
        m_Resolution(resolution)
    {
        if (resolution <= 0)
            throw std::invalid_argument("Shadow map resolution must be positive");

        glGenFramebuffers(1, &m_Framebuffer);
        glGenTextures(1, &m_DepthTexture);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH_COMPONENT24,
            resolution,
            resolution,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            m_DepthTexture,
            0
        );
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (!complete)
            throw std::runtime_error("Shadow map framebuffer is incomplete");
    }

    ShadowRenderer::~ShadowRenderer()
    {
        if (m_DepthTexture)
            glDeleteTextures(1, &m_DepthTexture);
        if (m_Framebuffer)
            glDeleteFramebuffers(1, &m_Framebuffer);
    }

    bool ShadowRenderer::Render(const Scene& scene)
    {
        const auto& lights = scene.GetDirectionalLights();
        if (lights.empty())
        {
            m_HasShadowMap = false;
            return false;
        }

        const glm::vec3 direction = lights.front()->GetDirection();
        if (glm::length(direction) < 0.0001f)
        {
            m_HasShadowMap = false;
            return false;
        }

        const glm::vec3 normalizedDirection = glm::normalize(direction);
        const glm::vec3 up = std::abs(glm::dot(normalizedDirection, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.98f
            ? glm::vec3(1.0f, 0.0f, 0.0f)
            : glm::vec3(0.0f, 1.0f, 0.0f);
        const glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 60.0f);
        const glm::mat4 lightView = glm::lookAt(
            -normalizedDirection * 25.0f,
            glm::vec3(0.0f),
            up
        );
        m_LightSpaceMatrix = lightProjection * lightView;

        GLint previousViewport[4];
        glGetIntegerv(GL_VIEWPORT, previousViewport);
        glViewport(0, 0, m_Resolution, m_Resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        m_DepthShader.Bind();
        m_DepthShader.SetUniformMat4("lightSpaceMatrix", m_LightSpaceMatrix);
        for (const auto& entity : scene.GetEntities())
        {
            if (!entity->IsEnabled() || entity->GetMaterial().IsTransparent())
                continue;

            m_DepthShader.SetUniformMat4(
                "model",
                entity->GetTransform().GetWorldMatrix()
            );
            entity->GetMesh().GetVertexArray().Bind();
            glDrawElements(
                GL_TRIANGLES,
                entity->GetMesh().GetIndexBuffer().GetCount(),
                GL_UNSIGNED_INT,
                nullptr
            );
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
        m_HasShadowMap = true;
        return true;
    }

    bool ShadowRenderer::HasShadowMap() const { return m_HasShadowMap; }
    const glm::mat4& ShadowRenderer::GetLightSpaceMatrix() const { return m_LightSpaceMatrix; }

    void ShadowRenderer::BindTexture(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
    }
}
