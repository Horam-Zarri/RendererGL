#include "Texture.hpp"
#include <stb_image.h>

#include "Core/Shader/Shader.hpp"
#include "Model/Model.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Renderer.hpp"


GLenum Texture::getInternalFormat(int nrComponents, bool srgb) {
    switch (nrComponents) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return srgb ? GL_SRGB : GL_RGB;
        case 4: return srgb ? GL_SRGB_ALPHA : GL_RGBA;
    }

    std::cerr << "TEXTRE:: Unknown component count: " << nrComponents << std::endl;
    return -1;
}

GLenum Texture::getDataFormat(int nrComponents) {
    switch (nrComponents) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
    }

    std::cerr << "TEXTRE:: Unknown component count: " << nrComponents << std::endl;
    return -1;
}

Texture::Texture(TextureType ttype, TextureConfig tconf):
    m_Type{ttype},
    m_Config{tconf}
{
    glGenTextures(1, &m_TextureID);
    std::cout << "TEX GENERATED FOR " << m_TextureID << std::endl;
}

Texture::Texture(
    const void* pixels,
    unsigned int width,
    unsigned int height,
    TextureType type,
    TextureConfig tconf
) :
    m_Pixels{pixels}, m_Path{}, m_Type{type}, m_Config{tconf}
{
    m_Width = width;
    m_Height = height;

    glGenTextures(1, &m_TextureID);
    genTexture();
}

Texture::Texture(const std::string& path, TextureType type, TextureConfig tconf)
: m_Path{path}, m_Type{type}, m_Config{tconf}
{
    glGenTextures(1, &m_TextureID);
    genTexture();
}

void Texture::genTexture() {
    if (!m_Path.empty())
        genFromFile();
    else
        genFromPixels();
}

void Texture::genFromPixels() {
    bind();

    glTexImage2D(
        GL_TEXTURE_2D,
        0, m_Config.internal_format,
        m_Width, m_Height,
        0, m_Config.data_format,
        m_Config.data_type,
        m_Pixels
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Config.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  m_Config.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Config.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Config.wrap_t);
}

void Texture::genFromFile() {
    stbi_set_flip_vertically_on_load(m_Config.flip);

    int nrComponents;
    unsigned char* dataC = nullptr;
    float* dataF = nullptr;

    if (m_Config.hdr)
        dataF = stbi_loadf(m_Path.c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);
    else
        dataC = stbi_load(m_Path.c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);

    if (dataC || dataF)
    {
        std::cout << "Found " << nrComponents << " components for " << m_Path << std::endl;

        GLenum internal_format = m_Config.internal_format;
        GLenum data_format = m_Config.data_format;

        // sentinel value for unspecified
        if (data_format == TextureConfig::UNSPECIFIED) {
            unsigned int chan = m_Config.nrChannels;
            if (chan == TextureConfig::UNSPECIFIED)
                chan = nrComponents;
            internal_format = getInternalFormat(chan, m_Config.srgb);
            data_format = getDataFormat(chan);
        }

        if (m_Config.hdr) {
            internal_format = GL_RGB16F;
            data_format = GL_RGB;
        }

        const void* _data;

        if (dataC) _data = dataC; else _data = dataF;

        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(
            GL_TEXTURE_2D,
            0, internal_format,
            m_Width,
            m_Height,
            0, data_format,
            m_Config.hdr ? GL_FLOAT : GL_UNSIGNED_BYTE,
            _data
        );

        if (m_Config.hdr) {
            m_Config.wrap_s = m_Config.wrap_t = GL_CLAMP_TO_EDGE;
            m_Config.min_filter = m_Config.mag_filter = GL_LINEAR;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Config.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Config.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Config.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Config.mag_filter);

        if (dataC) {
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(dataC);
        }
        if (dataF)
            stbi_image_free(dataF);
    }
    else {
        std::cout << "Texture failed to load at path: " << m_Path << std::endl;
        if (dataC) stbi_image_free(dataC);
        if (dataF) stbi_image_free(dataF);
    }
}

void Texture::bind() const {
    glActiveTexture(GL_TEXTURE0 + m_Slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::unbind() const {
    glActiveTexture(GL_TEXTURE_2D + m_Slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}
