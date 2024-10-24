#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <stb_image.h>

enum class TextureType {
    DIFFUSE, SPECULAR
};

struct TextureConfig {
    GLint level;
    GLint format_internal;
    GLint format;

    int mag_filter;
    int min_filter;
    int wrap_t;
    int wrap_s;
    int wrap_r;
};

class Texture {
private:
    TextureConfig m_TexConfig;
    unsigned int m_TextureID;
    unsigned char* m_Buffer;
    unsigned int m_Width, m_Height;

    bool m_Dirty;
public:
    std::string m_Path;
    TextureType m_Type;

    Texture();

    void init();
    void load(const std::string& path);
    void load(const std::string& path, TextureConfig texture_conf);
    void bind(unsigned int slot);
};

#endif
