#ifndef MODEL_H
#define MODEL_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"
#include "Core/Shader/shader.hpp"
#include "Texture/texture.hpp"
#include "Lighting/material.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>



class Model {
public:

    Model(const char* path);
    ~Model();

    void Draw(Shader& shader);

private:
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    void loadModel(std::string path);

    void processNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};

#endif
