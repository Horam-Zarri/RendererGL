#ifndef MONOBUFFER_TEXTURE_H
#define MONOBUFFER_TEXTURE_H

#include "Texture.hpp"

#include "Util/Ptr.hpp"
#include "Util/MoveOnly.hpp"

class MonoBufferTexture : public Texture {
    MAKE_MOVE_ONLY(MonoBufferTexture)
    GENERATE_PTR(MonoBufferTexture)

public:
    MonoBufferTexture(
        unsigned int width,
        unsigned int height
    );

    virtual void genTexture() override;

    void resize(unsigned int width, unsigned int height);
};

#endif
