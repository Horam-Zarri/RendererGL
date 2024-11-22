#include "ColorBufferTexture.hpp"

ColorBufferTexture::ColorBufferTexture(
    unsigned int width,
    unsigned int height,
    TextureConfig tconf
) :
    Texture(TextureType::ColorAttach, tconf)
{
    m_Width = width;
    m_Height = height;

    genTexture();
}

void ColorBufferTexture::genTexture() {
    bind();

    auto internal_format = m_Config.hdr ? GL_RGBA32F : GL_RGB;
    auto format = m_Config.hdr ? GL_RGBA : GL_RGB;
    auto type = format == GL_RGBA ? GL_FLOAT : GL_UNSIGNED_BYTE;

    glTexImage2D(
        GL_TEXTURE_2D,
        0, internal_format,
        m_Width,
        m_Height,
        0, format,
        type,
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Config.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Config.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Config.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Config.wrap_t);

    unbind();
}

void ColorBufferTexture::resize(unsigned int width, unsigned int height) {
    m_Width = width;
    m_Height = height;
    genTexture();
}
