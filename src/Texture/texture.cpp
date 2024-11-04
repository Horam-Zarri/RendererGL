#include "texture.hpp"
#include "Core/Shader/shader.hpp"
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

void Texture::gen_color_buffer(unsigned int width, unsigned int height) {
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

void Texture::gen_depth_stencil_buffer(unsigned int width, unsigned int height) {
    handle_dirty();

    m_Type = TextureType::DEPTH_STENCIL_ATTACH;

    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
                 GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::resize(unsigned int width, unsigned int height) {
    // somewhat straightforward solution for now
    switch(m_Type) {
        case TextureType::COLOR_ATTACH:
            gen_color_buffer(width, height);
            break;
        case TextureType::DEPTH_STENCIL_ATTACH:
            gen_depth_stencil_buffer(width, height);
            break;

        default:
            break;
    }
}

void Texture::load_file(const std::string& path, TextureConfig tex_conf) {
    handle_dirty();

    m_Path = path;

    // TODO: textures loaded from file do not currently need type
    // specified but remember to do something with it anyways
    m_Type = TextureType::DIFFUSE;

    int nrComponents;
    unsigned char *data = stbi_load(path.c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);
    if (data)
    {
        GLenum format_internal = tex_conf.format_internal;
        GLenum format = tex_conf.format;

        if (format == -1)
            format = format_internal = get_color_format(nrComponents);

        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format_internal, m_Width, m_Height,
                     0, format, GL_UNSIGNED_BYTE, data);
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

