#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <stb_image.h>

enum class TextureType {
    DIFFUSE, SPECULAR,
    COLOR_ATTACH, COLOR_ATTACH_MULTISAMPLE,
    DEPTH_STENCIL_ATTACH
};

static inline GLenum get_internal_format(int nrComponents, bool gamma_correction) {
    switch (nrComponents) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return gamma_correction ? GL_SRGB : GL_RGB;
        case 4: return gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
    }
}

static inline GLenum get_data_format(int nrComponents) {
    switch (nrComponents) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
    }
}

struct TextureConfig {
    GLint level;
    GLint internal_format;
    GLint data_format;

    int mag_filter;
    int min_filter;
    int wrap_t;
    int wrap_s;
    int wrap_r;

    bool gamma_correction;

    TextureConfig() {
        level = 0;

        // sentinel to state manual format detection
        data_format = internal_format = -1;

        mag_filter = GL_LINEAR;
        min_filter = GL_LINEAR_MIPMAP_LINEAR;

        wrap_t = wrap_s = wrap_r = GL_REPEAT;

        gamma_correction = true;
    }
};


class Texture {
private:
    unsigned int m_Width, m_Height;
    bool m_Dirty;

protected:
    unsigned int m_TextureID;
    // this is quite not a "pristine" kind of initialization handling
    void handle_dirty();

public:
    std::string m_Path;
    TextureType m_Type;

    Texture();

    void init();
    virtual void gen_color_buffer(unsigned int width, unsigned int height);
    virtual void gen_depth_stencil_buffer(unsigned int width, unsigned int height);

    virtual void resize(unsigned int width, unsigned height);

    void load_file(const std::string& path, TextureConfig tex_conf = TextureConfig());

    void bind() const;
    void bind(unsigned int slot) const;

    inline unsigned int id() const {
        return m_TextureID;
    }
};

#endif
