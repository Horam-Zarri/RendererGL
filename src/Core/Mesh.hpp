#ifndef POLYGON_H
#define POLYGON_H

#include "Core/IndexBuffer.hpp"
#include "Core/VertexArray.hpp"
#include "Core/VertexBuffer.hpp"
#include "Core/Vertex.hpp"

#include "Lighting/Material.hpp"

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

#include "Texture/Texture.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <vector>
#include <optional>

class Mesh {
    MAKE_MOVE_ONLY(Mesh)
    GENERATE_PTR(Mesh)
private:
    VertexBuffer::Ptr m_VBO;
    IndexBuffer::Ptr m_IBO;
    VertexArray::Ptr m_VAO;

    Material::Ptr m_Material;
    glm::mat4 m_ModelMatrix;

    std::vector<Texture::Ptr> m_Textures;

    unsigned int m_VerticesLength, m_IndicesLength;

    void init(
        std::vector<Vertex>& vertices,
        std::optional<std::reference_wrapper<std::vector<unsigned int>>> indices
    );

public:

    // TODO: make these cleaner
    Mesh(std::vector<Vertex>& vertices);
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices);
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture::Ptr>& textures);

    inline void translate(const glm::vec3& v) { m_ModelMatrix = glm::translate(m_ModelMatrix, v); }
    inline void rotate(float deg, const glm::vec3& v) { m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(deg), v); }
    inline void scale(const glm::vec3& v) { m_ModelMatrix = glm::scale(m_ModelMatrix, v); }

    inline void addTexture(const Texture::Ptr& texture) { m_Textures.push_back(texture); }
    inline void removeTexture(int index) { m_Textures.erase(m_Textures.begin() + index); }
    inline std::vector<Texture::Ptr>& getTextures() { return m_Textures; }

    inline void setMaterial(const Material::Ptr& material) { m_Material = material; }
    inline const Material::Ptr& getMaterial() { return m_Material; }

    inline void setModelMatrix(const glm::mat4& modelMatrix) { m_ModelMatrix = modelMatrix; }
    inline glm::mat4& getModelMatrix() { return m_ModelMatrix; }

    virtual void draw(bool wireframe = false, GLenum primitive = GL_TRIANGLES);
};

#endif
