#include "Mesh.hpp"
#include "Core/IndexBuffer.hpp"
#include "Core/Vertex.hpp"
#include "Core/VertexArray.hpp"

#include <functional>

Mesh::Mesh(std::vector<Vertex>& vertices):
    m_ModelMatrix(1.f),
    m_VerticesLength(vertices.size()), m_IndicesLength(0)
{
    init(vertices, std::nullopt);
}

Mesh::Mesh(
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices
) :
    m_ModelMatrix(1.f),
    m_VerticesLength(vertices.size()), m_IndicesLength(indices.size())
{
    init(vertices, indices);
}


Mesh::Mesh(
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    std::vector<Texture::Ptr>& textures
) :
    m_ModelMatrix(1.f),
    m_VerticesLength(vertices.size()), m_IndicesLength(indices.size()),
    m_Textures(textures)
{
    init(vertices, indices);
}

void Mesh::init(
    std::vector<Vertex>& vertices,
    std::optional<std::reference_wrapper<std::vector<unsigned int>>> indices
) {
    m_VBO = VertexBuffer::New();
    m_VAO = VertexArray::New();

    m_VBO->sendData(vertices.data(), sizeof(Vertex) * vertices.size());

    bool hasIndices = indices.has_value();

    if (hasIndices) {
        std::vector<unsigned int>& idxs = indices.value();

        m_IBO = IndexBuffer::New();
        m_IBO->sendData(idxs.data(), idxs.size());
    }

    m_VAO->sendLayout(m_VBO, defaultVertexLayout());

}

void Mesh::draw(bool wireframe) {
    m_VAO->bind();

    wireframe
        ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
        : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // TODO: 1 - Allow drawing more primitives
    //       2 - Add wireframe mode
    if (m_IBO == nullptr)
        glDrawArrays(GL_TRIANGLES, 0, m_VerticesLength * 3);
    else
        glDrawElements(GL_TRIANGLES, m_IndicesLength, GL_UNSIGNED_INT, 0);

}
