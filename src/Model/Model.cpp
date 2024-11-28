#include "Model.hpp"
#include "Lighting/Material.hpp"
#include "Lighting/PhongMaterial.hpp"
#include "assimp/material.h"
#include "assimp/postprocess.h"

#include <stb_image.h>


Model::Model(const std::string& path, bool pbr) {
    m_Pbr = pbr;
    loadModel(path);
}


void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    }
    m_Directory = path.substr(0, path.find_last_of('/'));

    std::cout << "ASSIMP::LOAD_MODEL" << std::endl;
    processNode(scene->mRootNode, scene);

    // Remove extra texture references
    m_TexturesLoaded.clear();
}

void Model::processNode(aiNode* node, const aiScene* scene) {

    //std::cout << "ASSIMP::NODE::" << node->mName.C_Str() << std::endl;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        //std::cout << "ASSIMP::MESH::" << scene->mMeshes[node->mMeshes[i]]->mName.C_Str() << std::endl;
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //std::cout << "AAAA" << std::endl;
        m_Meshes.push_back(processMesh(mesh, scene));
        //std::cout << "BBBB" << std::endl;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh::Ptr Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture::Ptr> textures;

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

        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;

        vertex.Tangent = vector;

        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;

        vertex.Bitangent = vector;

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
        aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];

        if (m_Pbr) {
            std::vector<Texture::Ptr> albedoMaps = loadMaterialTextures(
                mtl, aiTextureType_BASE_COLOR);
            textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());
            std::vector<Texture::Ptr> normalMaps = loadMaterialTextures(
                mtl, aiTextureType_NORMALS);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            std::vector<Texture::Ptr> metallicMaps = loadMaterialTextures(
                mtl, aiTextureType_METALNESS);
            textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
            std::vector<Texture::Ptr> roughnessMaps = loadMaterialTextures(
                mtl, aiTextureType_DIFFUSE_ROUGHNESS);
            textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
            std::vector<Texture::Ptr> aoMaps = loadMaterialTextures(
                mtl, aiTextureType_AMBIENT_OCCLUSION);
            textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
        }
        else {
            std::vector<Texture::Ptr> diffuseMaps = loadMaterialTextures(
                mtl, aiTextureType_DIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture::Ptr> specularMaps = loadMaterialTextures(
                mtl, aiTextureType_SPECULAR);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Texture::Ptr> normalMaps = loadMaterialTextures(
                mtl, aiTextureType_HEIGHT);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }
    }

    std::cout << "AFTER MATERIAL" << std::endl;

    Mesh::Ptr _mesh = Mesh::New(vertices, indices, textures);

    if (textures.empty()) {
        constexpr glm::vec3 DEFAULT_OBJ_COLOR(1.0f);
        _mesh->setMaterial(BasicMaterial::New(DEFAULT_OBJ_COLOR));
    } else {
        _mesh->setMaterial(PhongMaterial::New());
    }

    return _mesh;
}

std::vector<Texture::Ptr> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    std::vector<Texture::Ptr> textures;
    //std::cout << "ASSIMP::TEXTURE_COUNT::" << type << "::"  << mat->GetTextureCount(type) << std::endl;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;

        std::cout << "ASSIMP::TEXTURE_FILE::" << str.C_Str() << std::endl;
        for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++) {
            std::string fname = std::filesystem::path(m_TexturesLoaded[j]->m_Path).filename().string();
            if (std::strcmp(fname.data(), str.C_Str()) == 0)
            {
                //std::cout << "ASSIMP::TEXTURE_ALREADY_EXISTS" << std::endl;
                textures.push_back(m_TexturesLoaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {

            std::string fpath = std::string(str.C_Str());
            fpath = m_Directory + '/' + fpath;

            TextureType tx_type;

            switch (type)
            {
                case aiTextureType_DIFFUSE:
                    tx_type = TextureType::Diffuse;
                    break;
                case aiTextureType_SPECULAR:
                    tx_type = TextureType::Specular;
                    break;
                case aiTextureType_HEIGHT:
                case aiTextureType_NORMALS:
                    tx_type = TextureType::Normal;
                    break;
                case aiTextureType_BASE_COLOR:
                    tx_type = TextureType::Albedo;
                    std::cout << "MODE::BASE_TEXTURE" << std::endl;
                    break;
                case aiTextureType_METALNESS:
                    tx_type = TextureType::Metallic;
                    std::cout << "MODE::METALLIC_TEXTURE" << std::endl;
                    break;
                case aiTextureType_DIFFUSE_ROUGHNESS:
                    std::cout << "MODE::ROUGHNESS_TEXTURE" << std::endl;
                    tx_type = TextureType::Roughness;
                    break;
                case aiTextureType_AMBIENT_OCCLUSION:
                    std::cout << "MODE::AO_TEXTURE" << std::endl;
                    tx_type = TextureType::Ao;
                    break;
                default:
                    tx_type = TextureType::None;
            }

            Texture::Ptr tx = Texture::New(fpath, tx_type);

            if (type == aiTextureType_DIFFUSE)
                tx->setSlot(0);
            else
                tx->setSlot(1);

            textures.push_back(tx);
            m_TexturesLoaded.push_back(tx);
        }
    }

    return textures;
}
