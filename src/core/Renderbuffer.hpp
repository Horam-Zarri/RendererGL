#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "renderer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


enum class RBType {
    DEPTH_STENCIL,
    DEPTH_STENCIL_MULTISAMPLE
};

class Renderbuffer {
private:
    unsigned int m_BufferID;

public:
    RBType m_Type;

    Renderbuffer(RBType type, int width, int height): m_Type{type}
    {
        glGenRenderbuffers(1, &m_BufferID);
        resize(width, height);
    }

    void resize(unsigned int width, unsigned int height) const {
        bind();

        if (m_Type == RBType::DEPTH_STENCIL)
            glRenderbufferStorage(
                GL_RENDERBUFFER,
                GL_DEPTH24_STENCIL8,
                width,
                height
            );
        else if (m_Type == RBType::DEPTH_STENCIL_MULTISAMPLE)
            glRenderbufferStorageMultisample(
                GL_RENDERBUFFER,
                renderer::g_Engine.MSAA_MULTIPLIER,
                GL_DEPTH24_STENCIL8,
                width,
                height
            );

        unbind();
    }
    inline void bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, m_BufferID);
    }

    inline void unbind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    inline unsigned int id() const {
        return m_BufferID;
    }
};

#endif
