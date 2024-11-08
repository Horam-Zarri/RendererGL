#ifndef COLORBUFFER_TEXTURE_H
#define COLORBUFFER_TEXTURE_H

#include "Texture.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

class ColorBufferTexture : public Texture {
    MAKE_MOVE_ONLY(ColorBufferTexture)
    GENERATE_PTR(ColorBufferTexture)
public:

    ColorBufferTexture(unsigned int width, unsigned int height);

    virtual void genTexture() override;

    void resize(unsigned int width, unsigned int height);
};

#endif
