#include "GBuffer.hpp"
#include "Renderer/Renderer.hpp"

GBuffer::GBuffer(unsigned int width, unsigned int height)
    : FrameBuffer()
{

    TextureConfig tconf;
    tconf.min_filter = tconf.mag_filter = GL_NEAREST;
    tconf.wrap_s = tconf.wrap_t = GL_CLAMP_TO_EDGE;

    m_PositionBuffer = ColorBufferTexture::New(width, height, tconf);
    m_NormalBuffer = ColorBufferTexture::New(width, height, tconf);
    m_AlbedoSpecBuffer = ColorBufferTexture::New(width, height, tconf);
    m_PositionLightSpace = ColorBufferTexture::New(width, height, tconf);

    m_RBO = RenderBuffer::New(RBType::DEPTH_STENCIL, width, height);

    m_PositionBuffer->setSlot(renderer::TEXTURE_SLOT_DEFERRED_POSITION);
    m_NormalBuffer->setSlot(renderer::TEXTURE_SLOT_DEFERRED_NORMAL);
    m_AlbedoSpecBuffer->setSlot(renderer::TEXTURE_SLOT_DEFERRED_ALBEDOSPEC);
    m_PositionLightSpace->setSlot(renderer::TEXTURE_SLOT_DEFERRED_POSITION_LIGHT_SPACE);

    bind();

    attachTexture(GL_COLOR_ATTACHMENT0, m_PositionBuffer);
    attachTexture(GL_COLOR_ATTACHMENT1, m_NormalBuffer);
    attachTexture(GL_COLOR_ATTACHMENT2, m_AlbedoSpecBuffer);
    attachTexture(GL_COLOR_ATTACHMENT3, m_PositionLightSpace);

    setDrawBuffers({
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    });

    attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, m_RBO);

    unbind();
}

void GBuffer::bindTextures()
{
    // we do not want to bind the fbo itself
    m_PositionBuffer->bind();
    m_NormalBuffer->bind();
    m_AlbedoSpecBuffer->bind();
    m_PositionLightSpace->bind();
}

void GBuffer::resize(unsigned int width, unsigned int height)
{
    m_PositionBuffer->resize(width, height);
    m_NormalBuffer->resize(width, height);
    m_AlbedoSpecBuffer->resize(width, height);
    m_PositionLightSpace->resize(width, height);

    m_RBO->resize(width, height);
}
