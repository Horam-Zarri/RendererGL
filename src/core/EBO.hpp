#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class EBO {
private:
	unsigned int m_BufferID;
    unsigned int m_Count;
public:

    EBO(): m_Count(0) {
        glGenBuffers(1, &m_BufferID);
    }

	~EBO() {
        glDeleteBuffers(1, &m_BufferID);
	}

	void send_data(const unsigned int* data, unsigned int count, GLenum usage = GL_STATIC_DRAW)
    {
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, usage);
        m_Count = count;
    }

	void bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    }

	void unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

	inline unsigned int id() const { return m_BufferID; }
    inline unsigned int count() const { return m_Count; }
};

#endif
