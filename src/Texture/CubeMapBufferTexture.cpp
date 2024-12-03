#include "CubeMapBufferTexture.hpp"

CubeMapBufferTexture::CubeMapBufferTexture(
    unsigned int width,
    unsigned int height,
    TextureConfig tconf
) :
    Texture(TextureType::CubeMapAttach, tconf)
{
    m_Width = width;
    m_Height = height;

    genTexture();
}

void CubeMapBufferTexture::genTexture()
{
    bind();

    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, m_Config.internal_format,
            m_Width,
            m_Height,
            0, m_Config.data_format,
            GL_FLOAT,
            nullptr
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_Config.wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_Config.wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_Config.wrap_r);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_Config.min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_Config.mag_filter);

    if (m_Config.gen_mipmap)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void CubeMapBufferTexture::bind() const {
    glActiveTexture(GL_TEXTURE0 + m_Slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
}

void CubeMapBufferTexture::unbind() const {
    glActiveTexture(GL_TEXTURE0 + m_Slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
