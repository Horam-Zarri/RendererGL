#include "MultisampleTexture.hpp"

MultisampleTexture::MultisampleTexture(
    unsigned int width,
    unsigned int height
) :
    Texture::Texture(TextureType::ColorAttachMultisample, TextureConfig())
{
    m_Width = width;
    m_Height = height;

    genTexture();
}

void MultisampleTexture::genTexture() {

    bind();

    // lets just do this for now
    m_Config.hdr = true;

    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        m_Config.msaa_multiplier,
        m_Config.hdr ? GL_RGBA32F : GL_RGB,
        m_Width,
        m_Height,
        GL_TRUE
    );

    unbind();
}

void MultisampleTexture::resize(unsigned int width, unsigned int height) {
    m_Width = width;
    m_Height = height;

    switch (m_Type) {
        case TextureType::ColorAttachMultisample:
            genTexture();
            break;
        default:
            break;
    }
}


