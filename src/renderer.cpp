
#include "renderer.hpp"
#include "camera.hpp"
#include "core/VAO.hpp"
#include "core/VBO.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "mesh.hpp"
#include "shader.hpp"
#include <glm/vec3.hpp>
#include <memory>
#include <stb_image.h>
#include "model.hpp"
#include "window.hpp"

namespace renderer {

using camera::Camera;

Camera camera::g_Camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera camera::CAMERA_STATE(glm::vec3(0.0f, 0.0f, 3.0f));

EngineState g_Engine;
EngineState ENGINE_STATE;

static glm::mat4 model_transform(1.0f);
static glm::mat4 view_transform;
static glm::mat4 projection_transform;

static unsigned int offscr_fbo;
static unsigned int offscr_tex;

static VAO* quadVAO_ptr;

static unsigned int postprocess_fbo;

static std::unique_ptr<Shader> offscr_shader;
static std::unique_ptr<Shader> pp_shader;
static std::unique_ptr<Shader> shaderLC;

static std::unique_ptr<Model> jtp_model;

const glm::vec3 lightDir = glm::vec3(4.0f, 2.0f, 2.0f);

float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

VAO* lightCubeVAO_ptr;

void send_offscr_uniforms() {
    glm::mat4 md = glm::scale(model_transform, glm::vec3(0.2));
    md = glm::rotate(md, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    offscr_shader->setMat4("model", md);
    offscr_shader->setMat4("view", camera::g_Camera.GetViewMatrix());
    offscr_shader->setMat4("projection", glm::perspective(glm::radians(camera::g_Camera.Zoom),
                                                          ASPECT_RATIO, g_Engine.NEAR_PLANE, g_Engine.FAR_PLANE));
    offscr_shader->setVec3("viewPos", camera::g_Camera.Position);
}

void send_postprocess_uniforms() {
    pp_shader->setInt("screenTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, offscr_tex);
}
void offscr_pass() {
    offscr_shader->use();
    send_offscr_uniforms();

    glBindFramebuffer(GL_FRAMEBUFFER, offscr_fbo);
    glEnable(GL_DEPTH_TEST);

    glClearColor(g_Engine.CLEAR_COLOR.r,g_Engine.CLEAR_COLOR.g, g_Engine.CLEAR_COLOR.b, g_Engine.CLEAR_COLOR.a); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    jtp_model->Draw(*offscr_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setup_offscr_pass() {
    glGenFramebuffers(1, &offscr_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, offscr_fbo);

    glGenTextures(1, &offscr_tex);
    glBindTexture(GL_TEXTURE_2D, offscr_tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           offscr_tex, 0);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" <<
            std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void postprocess_pass() {
    pp_shader->use();
    send_postprocess_uniforms();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    quadVAO_ptr->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void setup_postprocess_pass() {
    glBindFramebuffer(GL_FRAMEBUFFER, offscr_fbo);

    VBLayout layout;

    layout.push<float>(2);
    layout.push<float>(2);

    static VBO quadVBO;
    quadVBO.send_data(quadVertices, sizeof quadVertices);
    static VAO quadVAO;
    quadVAO.send_data(quadVBO, layout);

    quadVAO_ptr = &quadVAO;

    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void render() {

    // render
    using namespace window;

    GLbitfield clr_enbl;

    glm::vec4& clr_color = g_Engine.CLEAR_COLOR;
    glClearColor(clr_color.r, clr_color.g, clr_color.b, clr_color.a);
    if (g_Engine.CLEAR_COLOR_BUF) clr_enbl |= GL_COLOR_BUFFER_BIT;
    if (g_Engine.CLEAR_DEPTH_BUF) clr_enbl |= GL_DEPTH_BUFFER_BIT;
    if (g_Engine.CLEAR_STENCIL_BUF) clr_enbl |= GL_STENCIL_BUFFER_BIT;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    offscr_shader->use();
    offscr_pass();

    pp_shader->use();
    postprocess_pass();
    glm::mat4 model(1.0f);
    glm::mat4 view = camera::g_Camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera::g_Camera.Zoom), (float)800 / 600, 0.1f, 100.0f);


    shaderLC->use();

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightDir);
    model = glm::scale(model, glm::vec3(0.5f));
    shaderLC->setMat4("model", model);
    shaderLC->setMat4("view", view);
    shaderLC->setMat4("projection", projection);

    lightCubeVAO_ptr->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int init() {

    // do something for this
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);

    Shader shader_l("./shaders/default_obj_vs.glsl", "./shaders/john_the_baptist_fs.glsl");
    Shader shaderLC_l("./shaders/light_cube_vs.glsl", "./shaders/light_cube_fs.glsl");
    Shader shader_pp("./shaders/screen_PP_vs.glsl", "./shaders/screen_PP_fs.glsl");

    //Model jtp_model("./assets/John_the_Baptist.obj");

    shader_l.use();

    shader_l.setInt("material.texture_diffuse1", 0);
    shader_l.setInt("material.texture_specular1", 1);

    shader_l.setVec3("light.direction", lightDir);
    shader_l.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader_l.setVec3("light.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
    shader_l.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader_l.setFloat("material.shininess", 32);


    shader_pp.use();

    shaderLC_l.use();


    VBLayout layout;
    layout.push<float>(3);

    static VAO lightCubeVAO;
    static VBO lightCubeVBO;

    lightCubeVAO.bind();
    lightCubeVBO.send_data(vertices, sizeof vertices);
    lightCubeVAO.send_data(lightCubeVBO, layout);

    lightCubeVAO_ptr = &lightCubeVAO;

    offscr_shader = std::make_unique<Shader>(shader_l);
    shaderLC = std::make_unique<Shader>(shaderLC_l);
    pp_shader = std::make_unique<Shader>(shader_pp);

    std::unique_ptr<Model> p(new Model("./assets/John_the_Baptist.obj"));
    jtp_model.swap(p);

    setup_offscr_pass();
    setup_postprocess_pass();

    return 0;
}

void update_state() {
    camera::g_Camera.update_camera(camera::CAMERA_STATE);


    g_Engine.CLEAR_COLOR = ENGINE_STATE.CLEAR_COLOR;

    ENGINE_STATE = g_Engine;
}
void terminate() {}
}


