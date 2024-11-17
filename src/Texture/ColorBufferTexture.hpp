#ifndef COLORBUFFER_TEXTURE_H
#define COLORBUFFER_TEXTURE_H

#include "Texture.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

class ColorBufferTexture : public Texture {
    MAKE_MOVE_ONLY(ColorBufferTexture)
    GENERATE_PTR(ColorBufferTexture)

private:
    static inline TextureConfig defaultConfig() {
        TextureConfig tconf;
        tconf.min_filter = tconf.mag_filter = GL_LINEAR;
        tconf.wrap_s = tconf.wrap_t = GL_CLAMP_TO_EDGE;
        return tconf;
    }
public:

    ColorBufferTexture(
        unsigned int width,
        unsigned int height,
        TextureConfig = defaultConfig()
    );

    virtual void genTexture() override;

    void resize(unsigned int width, unsigned int height);
};

#endif
