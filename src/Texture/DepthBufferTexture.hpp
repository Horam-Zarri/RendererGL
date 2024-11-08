#ifndef DEPTHBUFFER_TEXTURE_H
#define DEPTHBUFFER_TEXTURE_H

#include "Texture.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

class DepthBufferTexture : public Texture {
    MAKE_MOVE_ONLY(DepthBufferTexture)
    GENERATE_PTR(DepthBufferTexture)
public:

    DepthBufferTexture(unsigned int width, unsigned int height);

    virtual void genTexture() override;
};

#endif
