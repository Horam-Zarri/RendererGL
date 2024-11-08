#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


enum class RBType {
    DEPTH_STENCIL,
    DEPTH_STENCIL_MULTISAMPLE
};

class RenderBuffer {
    MAKE_MOVE_ONLY(RenderBuffer)
    GENERATE_PTR(RenderBuffer)
private:
    unsigned int m_BufferID;

public:
    RBType m_Type;

    RenderBuffer(RBType type, int width, int height);
    void resize(unsigned int width, unsigned int height) const;

    inline void bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, m_BufferID);
    }

    inline void unbind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    inline unsigned int getID() const {
        return m_BufferID;
    }
};

#endif
