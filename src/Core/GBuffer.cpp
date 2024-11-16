#include "GBuffer.hpp"


GBuffer::GBuffer(unsigned int width, unsigned int height)
    : FrameBuffer()
{
    m_PositionBuffer = ColorBufferTexture::New(width, height);
    m_NormalBuffer = ColorBufferTexture::New(width, height);
    m_AlbedoSpecBuffer = ColorBufferTexture::New(width, height);
    m_PositionLightSpace = ColorBufferTexture::New(width, height);

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

    unbind();
}
