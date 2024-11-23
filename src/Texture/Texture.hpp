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

    int data_type;

    int msaa_multiplier;

    bool gamma_correction;
    bool hdr;

    bool flip;

    TextureConfig() {
        level = 0;

        // sentinel to state manual format detection
        data_format = internal_format = -1;

        mag_filter = GL_LINEAR;
        min_filter = GL_LINEAR_MIPMAP_LINEAR;

        wrap_t = wrap_s = wrap_r = GL_REPEAT;

        data_type = GL_UNSIGNED_BYTE;

        msaa_multiplier = 4;

        gamma_correction = true;
        hdr = true;

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
    Albedo, Roughness, Ao, Metallic
};

class Texture {
    MAKE_MOVE_ONLY(Texture)
    GENERATE_PTR(Texture)

private:
    const void* m_Pixels;
protected:
    unsigned int m_Width, m_Height;
    unsigned int m_Slot;
    unsigned int m_TextureID;

    TextureType m_Type;
    TextureConfig m_Config;

    static GLenum getInternalFormat(int nrComponents, bool srgb);
    static GLenum getDataFormat(int nrComponents);

    Texture(TextureType ttype, TextureConfig tconf);

    virtual void genTexture();
public:

    std::string m_Path;

    Texture(
        const void* pixels,
        unsigned int width,
        unsigned int height,
        TextureType type = TextureType::None,
        TextureConfig tconf = TextureConfig()
    );

    Texture(
        const std::string& path,
        TextureType type = TextureType::Diffuse,
        TextureConfig tconf = TextureConfig()
    );

    void genFromFile();
    void genFromPixels();

    virtual void bind() const;
    virtual void unbind() const;

    inline unsigned int getID() const { return m_TextureID; }

    inline const std::string& getPath() const { return m_Path; }

    inline void setSlot(unsigned int slot) { m_Slot = slot; }
    inline unsigned int getSlot() const {return m_Slot;}

    inline void setType(TextureType type) { m_Type = type; }
    inline const TextureType& getType() const { return m_Type; }

    inline void setTextureConfig(const TextureConfig& tconf) { m_Config = tconf; }
    inline const TextureConfig& getTextureConfig() const { return m_Config; }
};

#endif
