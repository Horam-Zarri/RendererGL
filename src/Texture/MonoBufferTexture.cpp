#include "MonoBufferTexture.hpp"

MonoBufferTexture::MonoBufferTexture(
    unsigned int width,
    unsigned int height
):
    Texture(TextureType::ColorAttach, TextureConfig())
{
    m_Width = width;
    m_Height = height;

    genTexture();
}

void MonoBufferTexture::genTexture() {
    bind();

    glTexImage2D(
        GL_TEXTURE_2D,
        0, GL_RED,
        m_Width,
        m_Height,
        0, GL_RED,
        GL_FLOAT,
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unbind();
}

// TODO: make resize virtual among textures
void MonoBufferTexture::resize(unsigned int width, unsigned int height)
{
    m_Width = width;
    m_Height = height;

    genTexture();
}
