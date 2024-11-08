#ifndef POLYGON_H
#define POLYGON_H

#include <glm/glm.hpp>

#include "Core/IndexBuffer.hpp"
#include "Core/VertexArray.hpp"
#include "Core/VertexBuffer.hpp"
#include "Lighting/Material.hpp"
#include "Vertex.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"
#include "Texture/Texture.hpp"

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

    Mesh(std::vector<Vertex>& vertices);
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture::Ptr>& textures);

    inline void addTexture(const Texture::Ptr& texture) { m_Textures.push_back(texture); }
    inline void removeTexture(int index) { m_Textures.erase(m_Textures.begin() + index); }
    inline std::vector<Texture::Ptr>& getTextures() { return m_Textures; }

    inline void setMaterial(const Material::Ptr& material) { m_Material = material; }
    inline const Material::Ptr& getMaterial() { return m_Material; }

    inline void setModelMatrix(const glm::mat4& modelMatrix) { m_ModelMatrix = modelMatrix; }
    inline glm::mat4& getModelMatrix() { return m_ModelMatrix; }

    void draw(bool wireframe = false);
};

#endif
