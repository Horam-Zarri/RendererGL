#include "RenderBuffer.hpp"
#include "Renderer/Renderer.hpp"

RenderBuffer::RenderBuffer(RBType type, int width, int height): m_Type{type}
{
    glGenRenderbuffers(1, &m_BufferID);
    resize(width, height);
}

void RenderBuffer::resize(unsigned int width, unsigned int height) const {
    bind();

    if (m_Type == RBType::DEPTH_STENCIL)
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width,
            height
        );
    else if (m_Type == RBType::DEPTH_STENCIL_MULTISAMPLE)
        glRenderbufferStorageMultisample(
            GL_RENDERBUFFER,
            renderer::g_Engine.MSAA_MULTIPLIER,
            GL_DEPTH24_STENCIL8,
            width,
            height
        );
    else if (m_Type == RBType::DEPTH)
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT32,
            width,
            height
        );

    unbind();
}
