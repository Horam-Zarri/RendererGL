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
    ColorBufferTexture::Ptr m_PositionLightSpace;

public:
    GBuffer(unsigned int width, unsigned int height);
};

#endif
