#include "cubemap.hpp"
#include "Texture/texture.hpp"

Cubemap::Cubemap(): m_Dirty{true} {}


void Cubemap::init() {
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
    m_Dirty = false;
}
void Cubemap::load(std::array<std::string, 6> paths, TextureConfig tex_conf) {

    if (m_Dirty) {
        std::cerr << "Texture loaded before initialization!" << std::endl;
        std::exit(2);
    }

    m_Paths = paths;

    int nrComponents;

    unsigned char* data;
    for (unsigned int i = 0; i < 6; i++)
    {
        data = stbi_load(paths[i].c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);


        if (data) {
            GLenum internal_format = tex_conf.internal_format;
            GLenum data_format = tex_conf.data_format;

            if (data_format == -1) {
                internal_format = get_internal_format(nrComponents, tex_conf.gamma_correction);
                data_format = get_data_format(nrComponents);
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, m_Width,
                         m_Height, 0, data_format, GL_UNSIGNED_BYTE, data);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, tex_conf.mag_filter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, tex_conf.min_filter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, tex_conf.wrap_s);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, tex_conf.wrap_t);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, tex_conf.wrap_r);
        }
        else {
            std::cout << "Cubemap failed to load at paths: " << std::endl;
            stbi_image_free(data);
        }
    }
}

void Cubemap::bind(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
}
