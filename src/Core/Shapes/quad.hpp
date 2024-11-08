#ifndef QUAD_H
#define QUAD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/VertexBuffer.hpp"
#include "Core/VertexArray.hpp"

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

static constexpr float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

class Quad {
    MAKE_MOVE_ONLY(Quad)
    GENERATE_PTR(Quad)

private:
    VertexArray::Ptr m_VAO;
    VertexBuffer::Ptr m_VBO;
public:
    Quad()
    {
        m_VAO = VertexArray::New();
        m_VBO = VertexBuffer::New();

        VBLayout layout;

        layout.push<float>(2);
        layout.push<float>(2);

        m_VBO->sendData(quadVertices, sizeof quadVertices);
        m_VAO->sendLayout(m_VBO, layout);
    }

    void draw() {
        m_VAO->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_VAO->unbind();
    }
};

#endif
