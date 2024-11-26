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
    TextureConfig sk_conf;

    sk_conf.min_filter = sk_conf.mag_filter = GL_LINEAR;
    sk_conf.wrap_t = sk_conf.wrap_s = sk_conf.wrap_t = GL_CLAMP_TO_EDGE;

    sk_conf.flip = false;

    std::cout << "NOW SKYMAP" << std::endl;
    m_Skymap = CubeMapTexture::New(faces, sk_conf);
    std::cout << "SE POP IN" << std::endl;
    m_Skymap->setSlot(0);

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
    m_Skymap->bind();

    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    m_VAO->unbind();
}
