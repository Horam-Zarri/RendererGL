#include "texture.hpp"
#include "Core/Shader/shader.hpp"
#include "Model/model.hpp"
#include "Renderer/camera.hpp"
#include "Renderer/renderer.hpp"

Texture::Texture(): m_Dirty{true} {}

void Texture::init() {
    glGenTextures(1, &m_TextureID);
    m_Dirty = false;
}


void Texture::handle_dirty() {
    if (m_Dirty) {
        std::cerr << "TEXTURE:: Texture loaded/generated before initialization!" << std::endl;
        std::exit(5);
    }
}

void Texture::genColorBuffer(unsigned int width, unsigned int height) {
    handle_dirty();

    m_Type = TextureType::COLOR_ATTACH;

    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height,
        0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::genDepthBuffer(unsigned int width, unsigned int height) {
    handle_dirty();

    m_Type = TextureType::DEPTH_ATTACH;

    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::resize(unsigned int width, unsigned int height) {
    // somewhat straightforward solution for now
    switch(m_Type) {
        case TextureType::COLOR_ATTACH:
            genColorBuffer(width, height);
            break;
        case TextureType::DEPTH_ATTACH:
            genDepthBuffer(width, height);
            break;

        default:
            break;
    }
}

void Texture::loadFile(const std::string& path, TextureConfig tex_conf) {
    handle_dirty();

    m_Path = path;

    // TODO: textures loaded from file do not currently need type
    // specified but remember to do something with it anyways
    m_Type = TextureType::DIFFUSE;

    int nrComponents;
    unsigned char *data = stbi_load(path.c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);
    if (data)
    {
        GLenum internal_format = tex_conf.internal_format;
        GLenum data_format = tex_conf.data_format;

        // sentinel value for unspecified
        if (data_format == -1) {
            internal_format = get_internal_format(nrComponents, tex_conf.gamma_correction);
            data_format = get_data_format(nrComponents);
        }


        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_Width, m_Height,
                     0, data_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_conf.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_conf.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_conf.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_conf.mag_filter);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

