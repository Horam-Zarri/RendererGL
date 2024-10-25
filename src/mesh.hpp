#ifndef MESH_H
#define MESH_H

#include <cassert>
#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/EBO.hpp"
#include "core/VAO.hpp"
#include "core/material.hpp"
#include "core/texture.hpp"
#include "shader.hpp"

#include <string>
#include <vector>
using namespace std;


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
private:
    VAO m_VAO;
    VBO m_VBO;
    EBO m_EBO;

    unsigned int m_VertexCount;
    std::unique_ptr<Material> m_Material;

public:

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, std::unique_ptr<Material> mat)
        : m_VertexCount(vertices.size()), m_Material(std::move(mat))
    {
        VBLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);


        m_VAO.bind();
        m_VBO.bind();
        m_VBO.send_data(vertices.data(), sizeof(Vertex) * vertices.size());
        m_EBO.send_data(indices.data(), indices.size());
        m_EBO.bind();
        m_VAO.send_data(m_VBO, layout);
    }

    Mesh(vector<Vertex> vertices, std::unique_ptr<Material> mat)
        : m_VertexCount(vertices.size()), m_Material(std::move(mat))
    {
        VBLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);

        m_VBO.send_data(vertices.data(), sizeof(Vertex) * vertices.size());
        m_VAO.send_data(m_VBO, layout);

    }

    void Draw(Shader &shader)
    {
        m_VAO.bind();
        m_EBO.bind();

        // interface shall allow for no material custom cases
        if (m_Material)
            m_Material->send_uniforms(shader);

        // maybe find a better way to handle this
        if (m_EBO.count() == 0)
            glDrawArrays(GL_TRIANGLES, 0, m_VertexCount * 3);
        else {
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_EBO.count()), GL_UNSIGNED_INT, 0);
        }

        m_VAO.unbind();
        glActiveTexture(GL_TEXTURE0);
    }

};
#endif
