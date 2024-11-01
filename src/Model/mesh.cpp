#include "mesh.hpp"

// constructor
Mesh::Mesh(
    std::vector<Vertex> vertices,
    std::vector<unsigned int> indices,
    std::unique_ptr<Material> mat
): m_VertexCount(vertices.size()), m_Material(std::move(mat))
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

Mesh::Mesh(
    std::vector<Vertex> vertices,
    std::unique_ptr<Material> mat
) : m_VertexCount(vertices.size()), m_Material(std::move(mat))
{
    VBLayout layout;
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);

    m_VBO.send_data(vertices.data(), sizeof(Vertex) * vertices.size());
    m_VAO.send_data(m_VBO, layout);

}

void Mesh::Draw(Shader &shader)
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

