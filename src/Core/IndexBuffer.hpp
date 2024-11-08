#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class IndexBuffer {
    MAKE_MOVE_ONLY(IndexBuffer)
    GENERATE_PTR(IndexBuffer)

private:
	unsigned int m_BufferID;
public:

    IndexBuffer() {
        glGenBuffers(1, &m_BufferID);
    }

	~IndexBuffer() {
        glDeleteBuffers(1, &m_BufferID);
	}

	void sendData(const unsigned int* data, unsigned int count, GLenum usage = GL_STATIC_DRAW)
    {
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, usage);
    }

	void bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    }

	void unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

	inline unsigned int getID() const { return m_BufferID; }
};

#endif
