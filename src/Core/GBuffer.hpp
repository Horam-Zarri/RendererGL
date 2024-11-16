#ifndef GBUFFER_H
#define GBUFFER_H

#include "Core/FrameBuffer.hpp"
#include "Texture/ColorBufferTexture.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

class GBuffer : public FrameBuffer {
    MAKE_MOVE_ONLY(GBuffer)
    GENERATE_PTR(GBuffer)
private:
    ColorBufferTexture::Ptr m_PositionBuffer;
    ColorBufferTexture::Ptr m_NormalBuffer;
    ColorBufferTexture::Ptr m_AlbedoSpecBuffer;

public:
    GBuffer(unsigned int width, unsigned int height) : FrameBuffer() {
        m_PositionBuffer = ColorBufferTexture::New(width, height);
        m_NormalBuffer = ColorBufferTexture::New(width, height);
        m_AlbedoSpecBuffer = ColorBufferTexture::New(width, height);

        attachTexture(GL_COLOR_ATTACHMENT0, m_PositionBuffer);
        attachTexture(GL_COLOR_ATTACHMENT1, m_NormalBuffer);
        attachTexture(GL_COLOR_ATTACHMENT2, m_AlbedoSpecBuffer);

        setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
    }
};

#endif
