#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.hpp"

// I prefer a duplication over abstraction in this case

class Cubemap {
private:
    unsigned int m_TextureID;
    unsigned char* m_Buffer;
    unsigned int m_Width, m_Height;

    bool m_Dirty;
public:
    std::array<std::string, 6> m_Paths;

    Cubemap(): m_Dirty{true} {}


    void init() {
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
        m_Dirty = false;
    }
    void load(std::array<std::string, 6> paths, TextureConfig tex_conf = TextureConfig()) {

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
                GLenum format = get_color_format(nrComponents);

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, m_Width,
                             m_Height, 0, format, GL_UNSIGNED_BYTE, data);

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

    void bind(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
    }
};

#endif
