#ifndef MESHGROUP_H
#define MESHGROUP_H

#include <glad/glad.h>
#include "Core/Mesh.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

class MeshGroup {
    MAKE_MOVE_ONLY(MeshGroup)
    GENERATE_PTR(MeshGroup)

private:
    glm::mat4 m_ModelMatrix;
protected:
    std::vector<Mesh::Ptr> m_Meshes;

    unsigned int m_Primitive;
    bool m_Wireframe;

public:

    inline void translate(const glm::vec3& v) {
        for (const Mesh::Ptr& mesh : m_Meshes)
            mesh->translate(v);
    }

    inline void scale(const glm::vec3& v) {
        for (const Mesh::Ptr& mesh : m_Meshes)
            mesh->scale(v);
    }

    inline void rotate(float deg, const glm::vec3& v) {
        for (const Mesh::Ptr& mesh : m_Meshes)
            mesh->rotate(deg, v);
    }

    MeshGroup(unsigned int primitive = GL_TRIANGLES, bool wireframe = false):
        m_Primitive{primitive}, m_Wireframe{wireframe} {}

    inline void addMesh(const Mesh::Ptr& mesh) { m_Meshes.push_back(mesh); }
    inline void removeMesh(size_t index) { m_Meshes.erase(m_Meshes.begin() + index); }

    inline const std::vector<Mesh::Ptr>& getMeshes() const { return m_Meshes; }
};

#endif
