#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class VertexBuffer {
private:
    unsigned int m_BufferID;
public:
    VertexBuffer() {
        glGenBuffers(1, &m_BufferID);
    }

    ~VertexBuffer() {
        glDeleteBuffers(1, &m_BufferID);
    }

    void send_data(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }
    void bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    }

    void unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

#endif
