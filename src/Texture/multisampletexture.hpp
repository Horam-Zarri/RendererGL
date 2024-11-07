#ifndef MULTISAMPLE_TEXTURE
#define MULTISAMPLE_TEXTURE

#include "Texture/texture.hpp"
#include "Renderer/renderer.hpp"

class MultisampleTexture : public Texture {
public:
    MultisampleTexture() : Texture::Texture() {}

    virtual void genColorBuffer(unsigned int width, unsigned int height) override {

        handle_dirty();

        m_Type = TextureType::COLOR_ATTACH_MULTISAMPLE;

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TextureID);

        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            renderer::g_Engine.MSAA_MULTIPLIER,
            GL_RGB,
            width,
            height,
            GL_TRUE
        );

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    }

    virtual void resize(unsigned int width, unsigned int height) override {
        switch (m_Type) {
            case TextureType::COLOR_ATTACH_MULTISAMPLE:
                genColorBuffer(width, height);
            break;
            default:
            break;
        }
    }
};

#endif
