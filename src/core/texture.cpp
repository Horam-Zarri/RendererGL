#include "texture.hpp"


Texture::Texture(): m_Dirty{true} {}

void Texture::init() {
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    m_Dirty = false;
}


void Texture::load(const std::string& path, TextureConfig tex_conf) {
    if (m_Dirty) {
        std::cerr << "Texture loaded before initialization!" << std::endl;
    }
    m_Path = path;

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

void Texture::bind(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

