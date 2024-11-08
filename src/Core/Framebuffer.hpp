#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture/Texture.hpp"
#include "RenderBuffer.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class FrameBuffer {
    GENERATE_PTR(FrameBuffer)
    MAKE_MOVE_ONLY(FrameBuffer)
private:
    unsigned int m_FrameBufferID;
public:

    FrameBuffer();

    void attachTexture(int attachment_target, const Texture::Ptr& texture);
    void attachRenderBuffer(int attachent_target, const RenderBuffer::Ptr& rbo);

    void blitTo(const FrameBuffer::Ptr& other, unsigned int width, unsigned int height);

    inline void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }

    inline void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
#endif
