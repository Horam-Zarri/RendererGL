#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <vector>

#include "VertexBuffer.hpp"

struct VBElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOf(unsigned int type) {
        switch (type) {
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT: return 4;
            case GL_UNSIGNED_BYTE: return 1;
        }
        return 0;
    }
};

class VBLayout {
private:
    std::vector<VBElement> m_Elements;
    unsigned int m_Stride;

public:

    VBLayout(): m_Stride(0) {}

    template<typename T>
    void push(unsigned int count) {
        static_assert(false);
    }

    template<>
    void push<float>(unsigned int count) {
        m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
        m_Stride += count * VBElement::getSizeOf(GL_FLOAT);
    }

    template<>
    void push<unsigned int>(unsigned int count) {
        m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
        m_Stride += count * VBElement::getSizeOf(GL_UNSIGNED_INT);
    }

    template<>
    void push<unsigned char>(unsigned int count) {
        m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_FALSE});
        m_Stride += count * VBElement::getSizeOf(GL_UNSIGNED_BYTE);
    }

    inline const std::vector<VBElement> elements() const {return m_Elements;}
    inline unsigned int stride() const {return m_Stride;}
};

class VertexArray {
private:
    unsigned int m_BufferID;

public:

    VertexArray() {
        glGenVertexArrays(1, &m_BufferID);
    }

    ~VertexArray() {
        glDeleteVertexArrays(1, &m_BufferID);
    }

    void send_data(const VertexBuffer& vbo, VBLayout layout) {
        bind();
        vbo.bind();
        size_t offset = 0;
        const auto& elements = layout.elements();

        unsigned int lc = 0;
        for (int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];
            glEnableVertexAttribArray(lc);
            glVertexAttribPointer(lc, element.count, element.type,
                                  element.normalized, layout.stride(), (void*)offset);
            offset += element.count * VBElement::getSizeOf(element.type);
            lc++;
        }
    }

    void bind() const {
        glBindVertexArray(m_BufferID);
    }

    void unbind() const {
        glBindVertexArray(0);
    }
};
#endif
