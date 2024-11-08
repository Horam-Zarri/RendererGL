#include "DepthBufferTexture.hpp"

DepthBufferTexture::DepthBufferTexture(unsigned int width, unsigned int height)
{
    m_Width = width;
    m_Height = height;
    m_Type = TextureType::DepthAttach;

    genTexture();
}

void DepthBufferTexture::genTexture() {
    bind();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    unbind();
}
