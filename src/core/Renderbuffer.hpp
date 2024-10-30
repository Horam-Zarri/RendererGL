#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>


enum class RBType {
    DEPTH_STENCIL
};

class Renderbuffer {
private:
    unsigned int m_BufferID;

public:
    RBType m_Type;

    Renderbuffer(RBType type, int width, int height): m_Type{type}
    {
        glGenRenderbuffers(1, &m_BufferID);
        bind();
        // TODO: storage based on RBType
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
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
