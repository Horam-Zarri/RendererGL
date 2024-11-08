#include "MultisampleTexture.hpp"
#include "Renderer/Renderer.hpp"

MultisampleTexture::MultisampleTexture(unsigned int width, unsigned int height)
    : Texture::Texture()
{
    m_Width = width;
    m_Height = height;
    m_Type = TextureType::ColorAttachMultisample;

    genTexture();
}

void MultisampleTexture::genTexture() {

    bind();

    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        renderer::g_Engine.MSAA_MULTIPLIER,
        GL_RGB,
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


