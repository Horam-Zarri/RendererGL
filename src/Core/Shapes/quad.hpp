#ifndef QUAD_H
#define QUAD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/VertexBuffer.hpp"
#include "Core/VertexArray.hpp"

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
public:
    Quad() {
        VBLayout layout;

        layout.push<float>(2);
        layout.push<float>(2);

        m_VBO.send_data(quadVertices, sizeof quadVertices);
        m_VAO.send_data(m_VBO, layout);
    }

    void Draw() {
        m_VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_VAO.unbind();
    }
private:
    VertexArray m_VAO;
    VertexBuffer m_VBO;
};

#endif
