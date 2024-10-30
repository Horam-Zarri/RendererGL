#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "core/Renderbuffer.hpp"
#include "core/texture.hpp"
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
                               GL_TEXTURE_2D, texture.id(), 0);
    }

    void attachRenderBuffer(int attachent_target, Renderbuffer& rbo) {
        bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachent_target,
                                  GL_RENDERBUFFER, rbo.id());
    }

    inline void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }

    inline void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
#endif
