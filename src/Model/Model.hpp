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

private:
    std::vector<Texture::Ptr> m_TexturesLoaded;
    std::string m_Directory;
    bool m_Pbr;

    void loadModel(std::string path);

    void processNode(aiNode* node, const aiScene* scene);
    Mesh::Ptr processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture::Ptr> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};

#endif
