#ifndef MESH_H
#define MESH_H

#include <cassert>
#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/IndexBuffer.hpp"
#include "Core/VertexArray.hpp"
#include "Lighting/material.hpp"
#include "Texture/texture.hpp"
#include "Core/Shader/shader.hpp"

#include <string>
#include <vector>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
private:
    VertexArray m_VAO;
    VertexBuffer m_VBO;
    IndexBuffer m_EBO;

    unsigned int m_VertexCount;
    std::unique_ptr<Material> m_Material;

public:

    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
         std::unique_ptr<Material> mat);

    Mesh(std::vector<Vertex> vertices, std::unique_ptr<Material> mat);

    void Draw(Shader &shader);
};
#endif
