#include "Skybox.hpp"
#include "Renderer.hpp"
#include "Renderer/Camera.hpp"
#include "Texture/Texture.hpp"
#include <stb_image.h>


static constexpr float skybox_vertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


Skybox::Skybox(std::array<std::string, 6> faces)
{
    TextureConfig sk_conf = defaultConfig();

    m_SkymapTexture = CubeMapTexture::New(faces, sk_conf);
    m_SkymapTexture->setSlot(0);

    m_VBO = VertexBuffer::New();
    m_VAO = VertexArray::New();

    VBLayout layout;
    layout.push<float>(3);

    m_VBO->sendData(skybox_vertices, sizeof skybox_vertices);
    m_VAO->sendLayout(m_VBO, layout);
}

Skybox::Skybox(const CubeMapBufferTexture::Ptr& cubemap_buffer)
{
    m_SkymapBuffer = cubemap_buffer;

    m_VBO = VertexBuffer::New();
    m_VAO = VertexArray::New();

    VBLayout layout;
    layout.push<float>(3);

    m_VBO->sendData(skybox_vertices, sizeof skybox_vertices);
    m_VAO->sendLayout(m_VBO, layout);
}

void Skybox::draw()
{
    m_VAO->bind();

    if (m_SkymapTexture) m_SkymapTexture->bind();
    if (m_SkymapBuffer) m_SkymapBuffer->bind();

    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    m_VAO->unbind();
}
