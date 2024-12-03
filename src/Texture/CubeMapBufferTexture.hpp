#ifndef CUBEMAP_BUFFER_TEXTURE_H
#define CUBEMAP_BUFFER_TEXTURE_H

#include "Texture.hpp"
#include "Util/Ptr.hpp"
#include "Util/MoveOnly.hpp"

class CubeMapBufferTexture : public Texture {
    MAKE_MOVE_ONLY(CubeMapBufferTexture)
    GENERATE_PTR(CubeMapBufferTexture)
public:
    static inline TextureConfig defaultConfig() {
        TextureConfig tconf;
        tconf.wrap_s = tconf.wrap_t = tconf.wrap_r = GL_CLAMP_TO_EDGE;
        tconf.min_filter = tconf.mag_filter = GL_LINEAR;
        tconf.internal_format = GL_RGB16F;
        tconf.data_format = GL_RGB;
        return tconf;
    }

    CubeMapBufferTexture(
        unsigned int width,
        unsigned int height,
        TextureConfig = defaultConfig()
    );

    virtual void genTexture() override;


    void bind() const override;
    void unbind() const override;
};

#endif
