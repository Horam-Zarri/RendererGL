#ifndef TEXTURE_H
#define TEXTURE_H

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>


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
    bool flip;

    TextureConfig() {
        level = 0;

        // sentinel to state manual format detection
        data_format = internal_format = -1;

        mag_filter = GL_LINEAR;
        min_filter = GL_LINEAR_MIPMAP_LINEAR;

        wrap_t = wrap_s = wrap_r = GL_REPEAT;

        gamma_correction = true;
        flip = true;
    }
};

enum class TextureType {
    None,
    Diffuse, Specular,
    Normal,
    CubeMap,
    ColorAttach, ColorAttachMultisample,
    DepthAttach,
};

class Texture {
    MAKE_MOVE_ONLY(Texture)
    GENERATE_PTR(Texture)

protected:
    unsigned int m_Width, m_Height;
    unsigned int m_Slot;
    unsigned int m_TextureID;

    TextureType m_Type;
    TextureConfig m_Config;

    static GLenum getInternalFormat(int nrComponents, bool gamma_correction);
    static GLenum getDataFormat(int nrComponents);

    Texture();

    virtual void genTexture();
public:

    std::string m_Path;

    Texture(
        const std::string& path,
        TextureType type = TextureType::Diffuse,
        TextureConfig tex_conf = TextureConfig()
    );


    //virtual void genColorBuffer(unsigned int width, unsigned int height);
    //virtual void genDepthBuffer(unsigned int width, unsigned int height);


    void genFromFile();

    void loadFile(const std::string& path, TextureConfig tex_conf = TextureConfig());

    virtual void bind() const;
    virtual void unbind() const;

    inline unsigned int getID() const { return m_TextureID; }

    inline const std::string& getPath() const { return m_Path; }

    inline void setSlot(unsigned int slot) { m_Slot = slot; }
    inline unsigned int getSlot() const {return m_Slot;}

    inline void setType(TextureType type) { m_Type = type; }
    inline const TextureType& getType() const { return m_Type; }
};

#endif
