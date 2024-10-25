#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.hpp"

// I prefer a duplication over abstraction in this case

class Cubemap {
private:
    TextureConfig m_TexConfig;
    unsigned int m_TextureID;
    unsigned char* m_Buffer;
    unsigned int m_Width, m_Height;

    bool m_Dirty;
public:
    std::string m_Paths[6];

    Cubemap(): m_Dirty{true} {}


    void init() {
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
    }

    void load(const std::string paths[6]) {

        if (m_Dirty) {
            std::cerr << "Texture loaded before initialization!" << std::endl;
            std::exit(2);
        }

        for (int i = 0; i < 6; i++)
            m_Paths[i] = paths[i];

        m_TexConfig.level = 0;
        m_TexConfig.wrap_s = GL_REPEAT;
        m_TexConfig.wrap_t = GL_REPEAT;
        m_TexConfig.wrap_r = GL_REPEAT;
        m_TexConfig.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        m_TexConfig.mag_filter = GL_LINEAR;

        int nrComponents;

        unsigned char* data;
        for (unsigned int i = 0; i < 6; i++)
        {
            data = stbi_load(paths[i].c_str(), (int*)&m_Width, (int*)&m_Height, &nrComponents, 0);


            if (data) {
                GLenum format = get_color_format(nrComponents);

                m_TexConfig.format_internal = m_TexConfig.format = format;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, m_Width,
                             m_Height, 0, format, GL_UNSIGNED_BYTE, data);

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_TexConfig.mag_filter);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_TexConfig.min_filter);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_TexConfig.wrap_s);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_TexConfig.wrap_t);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_TexConfig.wrap_r);
            }
            else {
                std::cout << "Cubemap failed to load at paths: " << m_Paths << std::endl;
                stbi_image_free(data);
            }
        }
    }
};

#endif
