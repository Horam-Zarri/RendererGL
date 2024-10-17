#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class VBO {
private:
    unsigned int m_BufferID;
public:
    VBO() {
        glGenBuffers(1, &m_BufferID);
    }

    ~VBO() {
        std::cout << "OYA" << std::endl;
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
