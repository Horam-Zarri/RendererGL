#include "CubeMapTexture.hpp"

#include <stb_image.h>
#include "Texture/Texture.hpp"

CubeMapTexture::CubeMapTexture(const std::array<std::string, 6>& faces, TextureConfig tex_conf):
    Texture(), m_Paths{faces}
{
    m_Config = tex_conf;
    m_Type = TextureType::CubeMap;

    genTexture();
}

void CubeMapTexture::genTexture() {

    stbi_set_flip_vertically_on_load(m_Config.flip);

    bind();

    int nrComponents;
    unsigned char* data;
    for (unsigned int i = 0; i < 6; i++)
    {
        data = stbi_load(m_Paths[i].c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);

        std::cout << "MWIDTH: " << m_Width << "MHEIGHT: " << m_Height << std::endl;

        if (data) {
            GLenum internal_format = m_Config.internal_format;
            GLenum data_format = m_Config.data_format;

            if (data_format == -1) {
                internal_format = getInternalFormat(nrComponents, m_Config.gamma_correction);
                data_format = getDataFormat(nrComponents);
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, m_Width,
                         m_Height, 0, data_format, GL_UNSIGNED_BYTE, data);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_Config.mag_filter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_Config.min_filter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_Config.wrap_s);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_Config.wrap_t);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_Config.wrap_r);
        }
        else {
            std::cout << "Cubemap failed to load at paths: " << std::endl;
            stbi_image_free(data);
        }
    }
}

void CubeMapTexture::bind() const {
    glActiveTexture(GL_TEXTURE0 + m_Slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
}

void CubeMapTexture::unbind() const {
    glActiveTexture(GL_TEXTURE0 + m_Slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

