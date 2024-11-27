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
    unsigned char *data = stbi_load(m_Path.c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);
    if (data)
    {
        GLenum internal_format = m_Config.internal_format;
        GLenum data_format = m_Config.data_format;

        // sentinel value for unspecified
        if (data_format == -1) {
            bool srgb = m_Config.gamma_correction;
            internal_format = getInternalFormat(nrComponents, srgb);
            data_format = getDataFormat(nrComponents);
        }


        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_Width, m_Height,
                     0, data_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Config.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Config.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Config.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Config.mag_filter);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << m_Path << std::endl;
        stbi_image_free(data);
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
