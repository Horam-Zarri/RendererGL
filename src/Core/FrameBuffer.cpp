#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer() {
    glGenFramebuffers(1, &m_FrameBufferID);
}

void FrameBuffer::attachTexture(int attachment_target, const Texture::Ptr& texture)
{
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_target,
                           texture->getType() == TextureType::ColorAttachMultisample
                           ? GL_TEXTURE_2D_MULTISAMPLE
                           : GL_TEXTURE_2D,
                           texture->getID(), 0);
}

void FrameBuffer::attachCubemapTexture(int attachment_target, const Texture::Ptr& texture, int face_slot)
{
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_target,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_slot,
                           texture->getID(), 0);
}

void FrameBuffer::attachRenderBuffer(int attachent_target, const RenderBuffer::Ptr& rbo)
{
    bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachent_target,
                              GL_RENDERBUFFER, rbo->getID());
}

void FrameBuffer::setDrawBuffers(const std::vector<unsigned int>& draw_buffs)
{
    bind();
    glDrawBuffers(draw_buffs.size(), draw_buffs.data());
}

void FrameBuffer::blitColorTo(const FrameBuffer::Ptr& other, unsigned int width, unsigned int height)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other->m_FrameBufferID);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::blitDepthTo(const FrameBuffer::Ptr& other, unsigned int width, unsigned int height)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other->m_FrameBufferID);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
