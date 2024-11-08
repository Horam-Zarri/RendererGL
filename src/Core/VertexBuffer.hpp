#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VertexBuffer {
    MAKE_MOVE_ONLY(VertexBuffer)
    GENERATE_PTR(VertexBuffer)


private:
    unsigned int m_BufferID;

public:
    VertexBuffer() {
        glGenBuffers(1, &m_BufferID);
    }

    void sendData(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }

    ~VertexBuffer() {
        glDeleteBuffers(1, &m_BufferID);
    }

    void bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    }

    void unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

#endif
