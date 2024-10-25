#include "skybox.hpp"
#include "renderer.hpp"
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


    m_Skymap.init();
    stbi_set_flip_vertically_on_load(false);
    m_Skymap.load(faces, sk_conf);
    stbi_set_flip_vertically_on_load(true);
    m_Skymap.bind(0);

    VBLayout layout;
    layout.push<float>(3);

    m_VBO.send_data(skybox_vertices, sizeof skybox_vertices);
    m_VBO.bind();
    m_VAO.send_data(m_VBO, layout);
    m_VAO.bind();
}
void Skybox::Draw(Shader& shader, const renderer::camera::Camera& camera) {
    glDepthFunc(GL_LEQUAL);

    shader.use();
    shader.setFloat("skybox", 0);
    shader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
    shader.setMat4("projection", glm::perspective(glm::radians(camera.Zoom), renderer::ASPECT_RATIO, renderer::g_Engine.NEAR_PLANE, renderer::g_Engine.FAR_PLANE));

    m_VAO.bind();
    m_Skymap.bind(0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_VAO.unbind();

    glDepthFunc(GL_LESS);
}
