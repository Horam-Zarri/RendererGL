#include "texture.hpp"

Texture::Texture() {
    glGenTextures(1, &m_TextureID);
}

Texture::~Texture() {
    // TODO: deallocate texture
    //glDeleteTextures(1, &m_TextureID);
}

void Texture::load(const std::string& path) {

    m_TexConfig.level = 0;
    m_TexConfig.wrap_s = GL_REPEAT;
    m_TexConfig.wrap_t = GL_REPEAT;
    m_TexConfig.wrap_r = GL_REPEAT;
    m_TexConfig.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    m_TexConfig.mag_filter = GL_LINEAR;

    int nrComponents;
    unsigned char *data = stbi_load(path.c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        m_TexConfig.format_internal = m_TexConfig.format = format;

        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
}

void Texture::load(const std::string& path, TextureConfig texture_conf) {
    int nrComponents;
    unsigned char *data = stbi_load(path.c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;


        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_TexConfig.format_internal, m_Width, m_Height,
                     0, m_TexConfig.format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_TexConfig.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_TexConfig.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_TexConfig.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_TexConfig.mag_filter);

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

