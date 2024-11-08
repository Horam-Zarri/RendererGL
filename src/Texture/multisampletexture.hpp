#ifndef MULTISAMPLE_TEXTURE
#define MULTISAMPLE_TEXTURE

#include "Texture/Texture.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

class MultisampleTexture : public Texture {
    MAKE_MOVE_ONLY(MultisampleTexture)
    GENERATE_PTR(MultisampleTexture)
public:
    MultisampleTexture(unsigned int width, unsigned int height);

    virtual void genTexture() override;
    void resize(unsigned int width, unsigned int height);

    inline void bind() const override {
        // Multisample does not need slot access
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TextureID);
    }

    inline void unbind() const override {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    }
};

#endif
