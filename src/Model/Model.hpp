#ifndef MODEL_H
#define MODEL_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/MeshGroup.hpp"
#include "Core/Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include "Lighting/Material.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>



class Model : public MeshGroup {
    MAKE_MOVE_ONLY(Model)
    GENERATE_PTR(Model)

public:
    Model(const std::string& path, bool pbr = false);

    void addModelTexture(const Texture::Ptr& texture)
    {
        if (!isSingleMesh())
        {
            std::cout << "MODEL::ERROR::Custom texture set is only allowed for single mesh models";
            std::exit(11);
        }
        m_Meshes[0]->addTexture(texture);
    }
private:
    std::vector<Texture::Ptr> m_TexturesLoaded;
    std::string m_Directory;
    bool m_Pbr;

    void loadModel(std::string path);

    void processNode(aiNode* node, const aiScene* scene);
    Mesh::Ptr processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture::Ptr> loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    inline const std::vector<Texture::Ptr>& getLoadedTextures() const {
        return m_TexturesLoaded;
    }

    inline bool isSingleMesh() const {
        return m_Meshes.size() == 1;
    }

};

#endif
