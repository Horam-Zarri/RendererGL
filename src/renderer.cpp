
#include "renderer.hpp"
#include "GLFW/glfw3.h"
#include "camera.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "core/Renderbuffer.hpp"
#include "cube.hpp"
#include "quad.hpp"
#include "shader.hpp"
#include <glm/vec3.hpp>
#include <memory>
#include <stb_image.h>
#include "model.hpp"
#include "core/texture.hpp"
#include "core/Framebuffer.hpp"
#include "skybox.hpp"
#include "window.hpp"

namespace renderer {

using camera::Camera;

Camera camera::g_Camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera camera::CAMERA_STATE(glm::vec3(0.0f, 0.0f, 3.0f));

EngineState g_Engine;
EngineState ENGINE_STATE;

static glm::mat4 view_transform;
static glm::mat4 projection_transform;

// if MSAA is enabled we draw to msaa_fbo then blit to offscr_fbo
static std::unique_ptr<Framebuffer> msaa_fbo;
static std::unique_ptr<Renderbuffer> msaa_rbo;
static Texture msaa_tex;

// if MSAA is disabled we draw directly to offscr_fbo
static std::unique_ptr<Framebuffer> offscr_fbo;
static std::unique_ptr<Renderbuffer> offscr_rbo;
static Texture offscr_tex;

static std::unique_ptr<Quad> screen_quad;
static std::unique_ptr<Cube> dir_light_cube;

static std::unique_ptr<Shader> offscr_shader;
static std::unique_ptr<Shader> pp_shader;
static std::unique_ptr<Shader> light_cube_shader;
static std::unique_ptr<Shader> skybox_shader;

static std::unique_ptr<Model> jtp_model;
static std::unique_ptr<Skybox> skybox_model;



void send_offscr_uniforms() {
    glm::mat4 md = glm::mat4(1.0f);
    md = glm::translate(md, g_Engine.OBJECT_POS);
    md = glm::scale(md, glm::vec3(1.0));
    md = glm::rotate(md, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0));
    offscr_shader->setMat4("model", md);
    offscr_shader->setMat4("view", camera::g_Camera.GetViewMatrix());
    offscr_shader->setMat4("projection", glm::perspective(glm::radians(camera::g_Camera.Zoom),
                                                          ASPECT_RATIO, g_Engine.NEAR_PLANE, g_Engine.FAR_PLANE));
    offscr_shader->setVec3("viewPos", camera::g_Camera.Position);

    offscr_shader->setVec3("light.direction", g_Engine.LIGHT_POS);
    offscr_shader->setVec3("light.ambient", g_Engine.LIGHT_AMBIENT);
    offscr_shader->setVec3("light.diffuse", g_Engine.LIGHT_DIFFUSE);
    offscr_shader->setVec3("light.specular", g_Engine.LIGHT_SPECULAR);
}

void send_postprocess_uniforms() {
    pp_shader->setInt("screenTexture", 0);

    pp_shader->setBool("sharpen", g_Engine.SHARPNESS_ENBL);
    pp_shader->setFloat("sharpness", g_Engine.SHARPNESS_AMOUNT);

    pp_shader->setBool("blur", g_Engine.BLUR_ENBL);
    pp_shader->setBool("grayscale", g_Engine.GRAYSCALE_ENBL);

    offscr_tex.bind(0);
}

void offscr_pass() {
    glViewport(0, 0, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    offscr_shader->use();
    send_offscr_uniforms();

    if (g_Engine.MSAA_ENBL)
        msaa_fbo->bind();
    else
        offscr_fbo->bind();

    glEnable(GL_DEPTH_TEST);

    glClearColor(g_Engine.CLEAR_COLOR.r,g_Engine.CLEAR_COLOR.g, g_Engine.CLEAR_COLOR.b, g_Engine.CLEAR_COLOR.a); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    jtp_model->Draw(*offscr_shader);
    skybox_model->Draw(*skybox_shader, camera::g_Camera);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (g_Engine.MSAA_ENBL)
        msaa_fbo->blitTo(*offscr_fbo, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
}



void setup_offscr_pass() {

    offscr_fbo = std::make_unique<Framebuffer>();

    offscr_tex.init();
    offscr_tex.gen_color_buffer(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    offscr_rbo = std::unique_ptr<Renderbuffer>
        (new Renderbuffer(
            RBType::DEPTH_STENCIL,
            g_Engine.RENDER_WIDTH,
            g_Engine.RENDER_HEIGHT
        ));

    offscr_fbo->attachTexture(GL_COLOR_ATTACHMENT0, offscr_tex);
    offscr_fbo->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, *offscr_rbo);

    msaa_fbo = std::make_unique<Framebuffer>();

    msaa_tex.init();
    msaa_tex.gen_color_buffer(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    msaa_rbo = std::unique_ptr<Renderbuffer>
        (new Renderbuffer(
            RBType::DEPTH_STENCIL_MULTISAMPLE,
            g_Engine.RENDER_WIDTH,
            g_Engine.RENDER_HEIGHT
        ));

    msaa_fbo->attachTexture(GL_COLOR_ATTACHMENT0, msaa_tex);
    msaa_fbo->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, *msaa_rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" <<
            std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void postprocess_pass() {
    glViewport(0, 0, g_Engine.SCREEN_WIDTH, g_Engine.SCREEN_HEIGHT);

    pp_shader->use();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    send_postprocess_uniforms();
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    screen_quad->Draw();
}

void setup_postprocess_pass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    screen_quad = std::make_unique<Quad>();
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


    light_cube_shader->use();

    model = glm::mat4(1.0f);
    model = glm::translate(model, g_Engine.LIGHT_POS);
    model = glm::scale(model, glm::vec3(0.5f));
    light_cube_shader->setMat4("model", model);
    light_cube_shader->setMat4("view", view);
    light_cube_shader->setMat4("projection", projection);


    dir_light_cube->Draw();
}

int init() {

    // TODO: do something for this
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_STENCIL_TEST);

    // TODO: This whole init section is messed up
    Shader shader_l("./shaders/default_obj_vs.glsl", "./shaders/default_obj_fs.glsl");
    Shader shaderLC_l("./shaders/light_cube_vs.glsl", "./shaders/light_cube_fs.glsl");
    Shader shader_pp("./shaders/screen_PP_vs.glsl", "./shaders/screen_PP_fs.glsl");
    Shader shader_skb("./shaders/skybox_vs.glsl", "./shaders/skybox_fs.glsl");

    //Model jtp_model("./assets/John_the_Baptist.obj");

    shader_l.use();

    shader_l.setInt("material.texture_diffuse1", 0);
    shader_l.setInt("material.texture_specular1", 1);

    shader_l.setVec3("light.direction", g_Engine.LIGHT_POS);
    shader_l.setVec3("light.ambient", g_Engine.LIGHT_AMBIENT);
    shader_l.setVec3("light.diffuse", g_Engine.LIGHT_DIFFUSE);
    shader_l.setVec3("light.specular", g_Engine.LIGHT_SPECULAR);
    shader_l.setFloat("material.shininess", 32);


    shader_pp.use();

    shaderLC_l.use();


    dir_light_cube = std::make_unique<Cube>();

    offscr_shader = std::make_unique<Shader>(shader_l);
    light_cube_shader = std::make_unique<Shader>(shaderLC_l);
    pp_shader = std::make_unique<Shader>(shader_pp);
    skybox_shader = std::make_unique<Shader>(shader_skb);

    std::unique_ptr<Model> p(new Model("./assets/backpack.obj"));
    jtp_model.swap(p);

    const std::string base = "./tex/skybox/";
    const std::array<std::string, 6> faces = {
        base + "right.jpg",
        base + "left.jpg",
        base + "top.jpg",
        base + "bottom.jpg",
        base + "front.jpg",
        base + "back.jpg"
    };

    for (int j = 0; j < 6; j++)
        std::cout << "FACE::" << j << "::" << faces[j] << std::endl;

    std::unique_ptr<Skybox> sk(new Skybox(faces));
    skybox_model.swap(sk);

    setup_offscr_pass();
    setup_postprocess_pass();

    return 0;
}

void update_state() {
    camera::g_Camera.update_camera(camera::CAMERA_STATE);

    g_Engine.CLEAR_COLOR = ENGINE_STATE.CLEAR_COLOR;

    // model
    if (g_Engine.OBJECT_POS != ENGINE_STATE.OBJECT_POS)
        g_Engine.OBJECT_POS = ENGINE_STATE.OBJECT_POS;

    // lighting
    if (g_Engine.LIGHT_POS != ENGINE_STATE.LIGHT_POS)
        g_Engine.LIGHT_POS = ENGINE_STATE.LIGHT_POS;
    if (g_Engine.LIGHT_AMBIENT != ENGINE_STATE.LIGHT_AMBIENT)
        g_Engine.LIGHT_AMBIENT = ENGINE_STATE.LIGHT_AMBIENT;
    if (g_Engine.LIGHT_DIFFUSE != ENGINE_STATE.LIGHT_DIFFUSE)
        g_Engine.LIGHT_DIFFUSE = ENGINE_STATE.LIGHT_DIFFUSE;
    if (g_Engine.LIGHT_SPECULAR != ENGINE_STATE.LIGHT_SPECULAR)
        g_Engine.LIGHT_SPECULAR = ENGINE_STATE.LIGHT_SPECULAR;

    // postprocess
    if (g_Engine.SHARPNESS_ENBL != ENGINE_STATE.SHARPNESS_ENBL)
        g_Engine.SHARPNESS_ENBL = ENGINE_STATE.SHARPNESS_ENBL;
    if (g_Engine.SHARPNESS_AMOUNT != ENGINE_STATE.SHARPNESS_AMOUNT)
        g_Engine.SHARPNESS_AMOUNT = ENGINE_STATE.SHARPNESS_AMOUNT;
    if (g_Engine.BLUR_ENBL != ENGINE_STATE.BLUR_ENBL)
        g_Engine.BLUR_ENBL = ENGINE_STATE.BLUR_ENBL ;
    if (g_Engine.GRAYSCALE_ENBL != ENGINE_STATE.GRAYSCALE_ENBL)
        g_Engine.GRAYSCALE_ENBL = ENGINE_STATE.GRAYSCALE_ENBL;


    // must update msaa before resizing
    if (g_Engine.MSAA_ENBL != ENGINE_STATE.MSAA_ENBL)
        g_Engine.MSAA_ENBL = ENGINE_STATE.MSAA_ENBL;


    bool regen_buffers = false;

    if (g_Engine.MSAA_ENBL && (g_Engine.MSAA_MULTIPLIER != ENGINE_STATE.MSAA_MULTIPLIER)) {
        // resize implementation of Texture and Renderbuffer
        // handles MSAA change below
        g_Engine.MSAA_MULTIPLIER = ENGINE_STATE.MSAA_MULTIPLIER;
        regen_buffers = true;
    }

    if (g_Engine.RENDER_WIDTH != ENGINE_STATE.RENDER_WIDTH) {

        g_Engine.RENDER_WIDTH = ENGINE_STATE.RENDER_WIDTH;
        g_Engine.RENDER_HEIGHT = ENGINE_STATE.RENDER_HEIGHT;

        regen_buffers = true;
    }

    if (regen_buffers) {
        msaa_tex.resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
        msaa_rbo->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

        offscr_tex.resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
        offscr_rbo->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
    }

    if (g_Engine.SCREEN_WIDTH != ENGINE_STATE.SCREEN_WIDTH) {

        g_Engine.SCREEN_WIDTH = ENGINE_STATE.SCREEN_WIDTH;
        g_Engine.SCREEN_HEIGHT = ENGINE_STATE.SCREEN_HEIGHT;

        window::resize_window(g_Engine.SCREEN_WIDTH, g_Engine.SCREEN_HEIGHT);
    }

    ENGINE_STATE = g_Engine;
}
void terminate() {}
}
