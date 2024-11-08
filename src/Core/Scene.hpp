#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Core/Mesh.hpp"
#include "Core/MeshGroup.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

class Scene {
    MAKE_MOVE_ONLY(Scene)
    GENERATE_PTR(Scene)

private:
    std::vector<MeshGroup::Ptr> m_MeshGroups;

    glm::mat4 m_ModelMatrix;
public:

    Scene()
        : m_ModelMatrix(1.f)
    {}

    Scene(std::vector<MeshGroup::Ptr> mesh_groups)
        : m_MeshGroups(mesh_groups), m_ModelMatrix(1.f)
    {}

    ~Scene() = default;
    inline void addGroup(MeshGroup::Ptr& mesh_group) { m_MeshGroups.push_back(mesh_group); }
    inline void removeGroup(int index) { m_MeshGroups.erase(m_MeshGroups.begin() + index); }

    inline const std::vector<MeshGroup::Ptr>& getMeshGroups() { return m_MeshGroups; }

    inline void setModelMatrix(const glm::mat4& modelMatrix) { m_ModelMatrix= modelMatrix; }
    inline glm::mat4& getModelMatrix() { return m_ModelMatrix; }

};

#endif
