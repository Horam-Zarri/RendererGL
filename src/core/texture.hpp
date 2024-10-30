#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <stb_image.h>

enum class TextureType {
    DIFFUSE, SPECULAR,
    COLOR_ATTACH, DEPTH_STENCIL_ATTACH
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

    TextureConfig() {
        level = 0;

        // sentinel to state manual format detection
        format = format_internal = -1;

        mag_filter = GL_LINEAR;
        min_filter = GL_LINEAR_MIPMAP_LINEAR;

        wrap_t = wrap_s = wrap_r = GL_REPEAT;
    }
};

static inline GLenum get_color_format(int number_of_channels) {
    switch(number_of_channels) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default:
            std::cout << "WARN::Cannot identify color channel" << std::endl;
            return -1;
    }
}

class Texture {
private:
    unsigned int m_TextureID;
    unsigned int m_Width, m_Height;

    bool m_Dirty;

    // this is quite not a "pristine" kind of initialization handling
    void handle_dirty();
public:
    std::string m_Path;
    TextureType m_Type;

    Texture();

    void init();
    void gen_color_buffer(unsigned int width, unsigned int height);
    void gen_depth_stencil_buffer(unsigned int width, unsigned int height);

    void load_file(const std::string& path, TextureConfig tex_conf = TextureConfig());
    void bind();
    void bind(unsigned int slot);

    inline unsigned int id() const {
        return m_TextureID;
    }
};

#endif
