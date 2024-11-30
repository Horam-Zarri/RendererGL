#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture/Texture.hpp"
#include "RenderBuffer.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"
#include "imgui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

class FrameBuffer {
    GENERATE_PTR(FrameBuffer)
    MAKE_MOVE_ONLY(FrameBuffer)
private:
    unsigned int m_FrameBufferID;
public:

    FrameBuffer();

    void attachTexture(int attachment_target, const Texture::Ptr& texture);
    void attachCubemapTexture(int attachment_target, const Texture::Ptr& texture, int face_slot);
    void attachRenderBuffer(int attachent_target, const RenderBuffer::Ptr& rbo);

    void setDrawBuffers(const std::vector<unsigned int>& draw_buffs);

    void blitColorTo(const FrameBuffer::Ptr& other, unsigned int width, unsigned int height);
    void blitDepthTo(const FrameBuffer::Ptr& other, unsigned int width, unsigned int height);

    inline void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }

    inline void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
#endif
