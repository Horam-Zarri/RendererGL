#ifndef MESH_H
#define MESH_H

#include <cassert>
#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/EBO.hpp"
#include "core/VAO.hpp"
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

public:
    vector<Texture>      textures;


    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
        : m_VertexCount(vertices.size())
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

        this->textures = textures;
    }

    Mesh(vector<Vertex> vertices, vector<Texture> textures)
        : m_VertexCount(vertices.size())
    {
        VBLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);

        m_VBO.send_data(vertices.data(), sizeof(Vertex) * vertices.size());
        m_VAO.send_data(m_VBO, layout);

        this->textures = textures;
    }

    void Draw(Shader &shader)
    {
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;

        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            string number;
            string name;

            auto type = textures[i].m_Type;
            if(type == TextureType::DIFFUSE) {
                name = "texture_diffuse";
                number = std::to_string(diffuseNr++);
            }
            else if (type == TextureType::SPECULAR) {
                name = "texture_specular";
                number = std::to_string(specularNr++);
            }
            else
                std::cerr << "MESH::UNKNOWN_TEXTURETYPE" << std::endl;

            shader.setFloat(("material." + name + number).c_str(), i);
            textures[i].bind(i);
        }

        m_VAO.bind();
        m_EBO.bind();

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
