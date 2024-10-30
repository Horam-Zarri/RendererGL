#ifndef FBO_H
#define FBO_H

#include "core/texture.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class FBO {
private:
    unsigned int m_FrameBufferID;
public:

    FBO() {
        glGenFramebuffers(1, &m_FrameBufferID);
    }

    void attachTexture(int attachment_target, Texture& texture)
    {
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_target,
                               GL_TEXTURE_2D, texture.id(), 0);
    }

    void attachRenderBuffer(int attachent_target);

    inline void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }

    inline void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
#endif
