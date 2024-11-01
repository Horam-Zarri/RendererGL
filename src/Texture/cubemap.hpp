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

    Cubemap();

    void init();
    void load(std::array<std::string, 6> paths, TextureConfig tex_conf);

    void bind(int slot);
};

#endif
