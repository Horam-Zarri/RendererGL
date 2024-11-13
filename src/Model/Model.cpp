#include "Model.hpp"
#include "Lighting/Material.hpp"
#include "assimp/material.h"

#include <stb_image.h>


Model::Model(const std::string& path) {
    loadModel(path);
}


void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
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

    std::cout << "ASSIMP::NODE::" << node->mName.C_Str() << std::endl;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        std::cout << "ASSIMP::MESH::" << scene->mMeshes[node->mMeshes[i]]->mName.C_Str() << std::endl;
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "AAAA" << std::endl;
        m_Meshes.push_back(processMesh(mesh, scene));
        std::cout << "BBBB" << std::endl;
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
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture::Ptr> diffuseMaps = loadMaterialTextures(
            material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture::Ptr> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture::Ptr> normalMaps = loadMaterialTextures(
            material, aiTextureType_HEIGHT);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
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
    std::cout << "ASSIMP::TEXTURE_COUNT::" << type << "::"  << mat->GetTextureCount(type) << std::endl;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;

        std::cout << "ASSIMP::TEXTURE_FILE::" << str.C_Str() << std::endl;
        for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++) {
            std::string fname = std::filesystem::path(m_TexturesLoaded[j]->m_Path).filename().string();
            if (std::strcmp(fname.data(), str.C_Str()) == 0)
            {
                std::cout << "ASSIMP::TEXTURE_ALREADY_EXISTS" << std::endl;
                textures.push_back(m_TexturesLoaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {

            std::string fpath = std::string(str.C_Str());
            fpath = m_Directory + '/' + fpath;

            TextureType tex_type;

            if (type == aiTextureType_DIFFUSE)
                tex_type = TextureType::Diffuse;
            else if (type == aiTextureType_SPECULAR)
                tex_type = TextureType::Specular;
            else if (type == aiTextureType_HEIGHT)
                tex_type = TextureType::Normal;
            else
                tex_type = TextureType::None;

            Texture::Ptr tx = Texture::New(fpath, tex_type);

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
