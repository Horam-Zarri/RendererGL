#include "model.hpp"
#include "core/material.hpp"

#include <stb_image.h>


Model::Model(const char* path) {
    loadModel(path);
}

void Model::Draw(Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i]->Draw(shader);
}

Model::~Model() {
    std::cout << "MODEL::DEALLOCATION::" << std::endl;
}
void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    }
    directory = path.substr(0, path.find_last_of('/'));

    std::cout << "ASSIMP::LOAD_MODEL" << std::endl;
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {

    std::cout << "ASSIMP::NODE::" << node->mName.C_Str() << std::endl;
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        std::cout << "ASSIMP::MESH::" << scene->mMeshes[node->mMeshes[i]]->mName.C_Str() << std::endl;
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "AAAA" << std::endl;
        meshes.push_back(processMesh(mesh, scene));
        std::cout << "BBBB" << std::endl;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    std::cout << "ASSIMP::VERTEX_COUNT::" << mesh->mNumVertices << std::endl;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->mNormals != NULL) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
        } else {
            vector = glm::vec3(0.0f);
        }
        vertex.Normal = vector;

        if (mesh->mTextureCoords[0])
        {
            vector.x = mesh->mTextureCoords[0][i].x;
            vector.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = glm::vec2(vector.x, vector.y);
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    std::cout << "ASSIMP::FACE_COUNT::" << mesh->mNumFaces << std::endl;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                                aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material,
                                                                 aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(),
                        specularMaps.end());
    }

    std::cout << "AFTER MATERIAL" << std::endl;

    if (textures.empty()) {
        constexpr glm::vec3 DEFAULT_OBJ_COLOR(1.0f);
        return std::unique_ptr<Mesh>(
            new Mesh(vertices, indices, std::unique_ptr<PhongMaterial>
                     (new PhongMaterial(DEFAULT_OBJ_COLOR))));
    } else {
        std::cout << "AFTER MATERIAL 2" << std::endl;
        std::unique_ptr<Material> pm;

        if (textures.size() == 1)
            pm = std::make_unique<PhongMaterial>(textures[0]);
        else
            pm = std::make_unique<PhongMaterial>(textures[0], textures[1]);

        return std::unique_ptr<Mesh>(
            new Mesh(vertices, indices, std::move(pm)));
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    std::vector<Texture> textures;
    std::cout << "ASSIMP::TEXTURE_COUNT::" << type << "::"  << mat->GetTextureCount(type) << std::endl;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;

        std::cout << "ASSIMP::TEXTURE_FILE::" << str.C_Str() << std::endl;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            std::string fname = std::filesystem::path(textures_loaded[j].m_Path).filename().string();
            if (std::strcmp(fname.data(), str.C_Str()) == 0)
            {
                std::cout << "ASSIMP::TEXTURE_ALREADY_EXISTS" << std::endl;
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.init();
            std::string fpath = std::string(str.C_Str());
            fpath = directory + '/' + fpath;
            texture.load_file(fpath);
            if (type == aiTextureType_DIFFUSE)
                texture.m_Type = TextureType::DIFFUSE;
            else
                texture.m_Type = TextureType::SPECULAR;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}
