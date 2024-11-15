#include "ColorBufferTexture.hpp"

ColorBufferTexture::ColorBufferTexture(unsigned int width, unsigned int height)
{
    m_Width = width;
    m_Height = height;
    m_Type = TextureType::ColorAttach;

    genTexture();
}

void ColorBufferTexture::genTexture() {
    bind();

    glTexImage2D(
        GL_TEXTURE_2D,
        0, m_Config.hdr ? GL_RGBA32F : GL_RGB,
        m_Width,
        m_Height,
        0, m_Config.hdr ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    unbind();
}

void ColorBufferTexture::resize(unsigned int width, unsigned int height) {
    m_Width = width;
    m_Height = height;
    genTexture();
}
