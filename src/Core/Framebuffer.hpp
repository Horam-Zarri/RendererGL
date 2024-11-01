#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Core/Renderbuffer.hpp"
#include "Texture/texture.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Framebuffer {
private:
    unsigned int m_FrameBufferID;
public:

    Framebuffer() {
        glGenFramebuffers(1, &m_FrameBufferID);
    }

    void attachTexture(int attachment_target, Texture& texture)
    {
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_target,
            texture.m_Type == TextureType::COLOR_ATTACH_MULTISAMPLE
            ? GL_TEXTURE_2D_MULTISAMPLE
            : GL_TEXTURE_2D,
            texture.id(), 0);
    }

    void attachRenderBuffer(int attachent_target, Renderbuffer& rbo) {
        bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachent_target,
                                  GL_RENDERBUFFER, rbo.id());
    }

    void blitTo(Framebuffer& other, unsigned int width, unsigned int height) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other.m_FrameBufferID);

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    inline void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }

    inline void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
#endif
