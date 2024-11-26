#include "Renderer.hpp"
#include "Camera.hpp"

#include <GLFW/glfw3.h>

#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/FrameBuffer.hpp"
#include "Core/MeshGroup.hpp"
#include "Core/RenderBuffer.hpp"
#include "Core/Shader/Shader.hpp"
#include "Core/Shapes/Cube.hpp"
#include "Core/Shapes/Plane.hpp"
#include "Core/Shapes/Quad.hpp"

#include "Core/Shapes/Sphere.hpp"
#include "Lighting/PBRMaterial.hpp"
#include "Model/Model.hpp"

#include "Texture/ColorBufferTexture.hpp"
#include "Texture/DepthBufferTexture.hpp"
#include "Texture/MonoBufferTexture.hpp"
#include "Texture/Texture.hpp"
#include "Texture/MultisampleTexture.hpp"
#include "Lighting/PointLight.hpp"
#include "Lighting/SpotLight.hpp"
#include "Lighting/Material.hpp"
#include "Lighting/PhongMaterial.hpp"

#include "Skybox.hpp"
#include "Window.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "imgui.h"

#include <memory>
#include <ostream>
#include <random>
#include <stb_image.h>

namespace renderer {

using camera::Camera;

EngineState g_Engine;
EngineState ENGINE_STATE;

Camera camera::g_Camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera camera::CAMERA_STATE(glm::vec3(0.0f, 0.0f, 3.0f));

Shader::Ptr shaderDefault;
Shader::Ptr shaderPhong;
Shader::Ptr shaderPostProcess;
Shader::Ptr shaderSkybox;
Shader::Ptr shaderShadow;
Shader::Ptr shaderBlur;
Shader::Ptr shaderGBuffer;
Shader::Ptr shaderGLightPass;
Shader::Ptr shaderPbr;

std::vector<Scene::Ptr> g_Scenes;
DirectionalLight::Ptr g_SunLight;
std::vector<Light::Ptr> g_Lights;

glm::mat4 g_View;
glm::mat4 g_Proj;
glm::mat4 g_LightSpaceMatrix;

FrameBuffer::Ptr fboShadow;
FrameBuffer::Ptr fboOffscrMSAA;
FrameBuffer::Ptr fboOffscr;
FrameBuffer::Ptr fboBlurHoriz;
FrameBuffer::Ptr fboBlurVert;
FrameBuffer::Ptr fboSSAO;
FrameBuffer::Ptr fboSSAOBlur;

GBuffer::Ptr fboGBuffer;

RenderBuffer::Ptr rboOffscr;
RenderBuffer::Ptr rboOffscrMSAA;

DepthBufferTexture::Ptr texShadowmap;
ColorBufferTexture::Ptr texOffscr;
ColorBufferTexture::Ptr texOffscrBright;
MultisampleTexture::Ptr texOffscrMSAA;
ColorBufferTexture::Ptr texBlurHoriz;
ColorBufferTexture::Ptr texBlurVert;
MonoBufferTexture::Ptr texSSAO;
MonoBufferTexture::Ptr texSSAOBlur;
Texture::Ptr texSSAONoise;

Quad::Ptr screenQuad;
Cube::Ptr pointLightsCube;
Cube::Ptr spotLightsCube;

Skybox::Ptr skybox;


void sendLightUniforms(const Shader::Ptr& shader) {
    shader->use();

    shader->setBool("blinn", g_Engine.BLINN_ENBL);
    shader->setInt("pointLightsSize", getLightsCount(LightType::PointLight));
    shader->setInt("spotLightsSize", getLightsCount(LightType::SpotLight));

    unsigned int pointLightIndex = 0, spotLightIndex = 0;

    for (const Light::Ptr& light : g_Lights) {

        LightType light_type = light->getType();

        if (light_type == LightType::PointLight) {
            PointLight::Ptr pl = std::dynamic_pointer_cast<PointLight, Light>(light);

            std::string base = "pointLights[" + std::to_string(pointLightIndex) + "]";
            shader->setVec3(base + ".position", pl->getPosition());

            shader->setVec3(base + ".ambient", pl->getAmbient());
            shader->setVec3(base + ".diffuse", pl->getDiffuse());
            shader->setVec3(base + ".specular", pl->getPosition());

            Attenuation atten = pl->getAttenuation();

            shader->setFloat(base + ".constant", atten.constant);
            shader->setFloat(base + ".linear", atten.linear);
            shader->setFloat(base + ".quadratic", atten.quadratic);

            pointLightIndex++;
        }

        if (light_type == LightType::SpotLight) {

            SpotLight::Ptr sl = std::dynamic_pointer_cast<SpotLight, Light>(light);

            std::string base = "spotLights[" + std::to_string(spotLightIndex) + "]";
            shader->setVec3(base + ".position", sl->getPosition());
            shader->setVec3(base + ".direction", sl->getDirection());

            shader->setVec3(base + ".ambient", sl->getAmbient());
            shader->setVec3(base + ".diffuse", sl->getDiffuse());
            shader->setVec3(base + ".specular", sl->getSpecular());

            Attenuation atten = sl->getAttenuation();

            shader->setFloat(base + ".constant", atten.constant);
            shader->setFloat(base + ".linear", atten.linear);
            shader->setFloat(base + ".quadratic", atten.quadratic);

            shader->setFloat(base + ".cutOff", sl->getCutOff());
            shader->setFloat(base + ".outerCutOff", sl->getOuterCutOff());

            spotLightIndex++;
        }
    }

    shader->setInt("material.shininess", 32);

    shader->setVec3("directionalLight.direction", g_SunLight->getDirection());
    shader->setVec3("directionalLight.ambient", g_SunLight->getAmbient());
    shader->setVec3("directionalLight.diffuse", g_SunLight->getDiffuse());
    shader->setVec3("directionalLight.specular", g_SunLight->getSpecular());
}

void sendLightPbrUniforms(const Shader::Ptr& shader) {
    shader->use();

    shader->setInt("pointLightsSize", getLightsCount(LightType::PointLight));

    unsigned int pointLightIndex = 0;

    for (const Light::Ptr& light : g_Lights) {
        LightType light_type = light->getType();

        if (light_type == LightType::PointLight) {
            PointLight::Ptr pl = std::dynamic_pointer_cast<PointLight, Light>(light);
            std::string base = "pointLights[" + std::to_string(pointLightIndex) + "]";
            shader->setVec3(base + ".position", pl->getPosition());
            shader->setVec3(base + ".color", pl->getAveragedColor());
            pointLightIndex++;
        }
    }
}

void renderLightCubes(const Shader::Ptr& shader) {
    shader->use();
    for (unsigned int i = 0; i < g_Lights.size(); ++i) {

        const auto& light = g_Lights[i];

        shader->setVec3("lightColor", light->getAveragedColor());

        glm::mat4 md(1.0f);

        const LightType light_type = light->getType();
        if (light_type == LightType::PointLight) {

            PointLight* pl = dynamic_cast<PointLight*>(light.get());

            md = glm::translate(md, pl->getPosition());
            md = glm::scale(md, glm::vec3(.25f));


            shader->setMat4("model", md);
            shader->setMat4("view", g_View);
            shader->setMat4("projection", g_Proj);

            pointLightsCube->draw();

        } else if (light_type == LightType::SpotLight) {


            SpotLight* sl = dynamic_cast<SpotLight*>(light.get());

            md = glm::translate(md, sl->getPosition());
            md = glm::scale(md, glm::vec3(.25f));

            shader->setMat4("model", md);
            shader->setMat4("view", g_View);
            shader->setMat4("projection", g_Proj);

            spotLightsCube->draw();
        }
    }
}


void renderScenes(const Shader::Ptr& shader) {
    shader->use();
    texShadowmap->bind();

    for (const Scene::Ptr& scene : g_Scenes) {
        for (const MeshGroup::Ptr& mesh_group : scene->getMeshGroups()) {
            for (const Mesh::Ptr& mesh : mesh_group->getMeshes()) {

                glm::mat4 scene_model = glm::translate(scene->getModelMatrix(), g_Engine.OBJECT_POS);
                glm::mat4 model = scene_model * mesh->getModelMatrix();

                shader->setMat4("model", model);

                bool hasDiffuse = false, hasSpecular = false, hasNormal = false;

                for (const auto& texture : mesh->getTextures()) {

                    unsigned int slot = 0;

                    switch (texture->getType()) {
                        case TextureType::Diffuse:
                            slot = TEXTURE_SLOT_DIFFUSE;
                            hasDiffuse = true;
                            break;
                        case TextureType::Specular:
                            slot = TEXTURE_SLOT_SPECULAR;
                            hasSpecular = true;
                            break;
                        case TextureType::Normal:
                            slot = TEXTURE_SLOT_NORMAL;
                            hasNormal = true;
                            break;
                        default:
                            break;
                    }

                    texture->setSlot(slot);
                    texture->bind();
                }


                shader->setBool("hasDiffuse", hasDiffuse);
                shader->setBool("hasSpecular", hasSpecular);
                shader->setBool("hasNormal", hasNormal);

                const auto& material = mesh->getMaterial();

                if (material != nullptr) {
                    MaterialType mat_type = material->getType();
                    if (mat_type == MaterialType::Solid) {
                        BasicMaterial::Ptr basic_mat = std::dynamic_pointer_cast<BasicMaterial>(material);
                        shaderDefault->setVec3("obj_color", basic_mat->getObjColor());
                    }
                    else if (mat_type == MaterialType::Phong) {
                        PhongMaterial::Ptr phong_mat = std::dynamic_pointer_cast<PhongMaterial>(material);
                        shader->setVec3("material.ambient", phong_mat->getAmbient());
                        shader->setVec3("material.diffuse", phong_mat->getDiffuse());
                        shader->setVec3("material.specular", phong_mat->getSpecular());
                        shader->setFloat("material.shininess", phong_mat->getShininess());
                    }
                    else if (mat_type == MaterialType::PBR) {
                        PBRMaterial::Ptr pbr_mat = std::dynamic_pointer_cast<PBRMaterial>(material);
                        shader->setVec3("material.albedo", pbr_mat->getAlbedo());
                        shader->setFloat("material.metallic", pbr_mat->getMetallic());
                        shader->setFloat("material.roughness", pbr_mat->getRoughness());
                        shader->setFloat("material.ao", pbr_mat->getAo());
                    }
                }

                mesh->draw();
            }
        }
    }
}

void renderScenesDepth() {
    shaderShadow->use();

    for (const Scene::Ptr& scene : g_Scenes) {
        for (const MeshGroup::Ptr& mesh_group : scene->getMeshGroups()) {
            for (const Mesh::Ptr& mesh : mesh_group->getMeshes()) {

                glm::mat4 scene_model = glm::translate(scene->getModelMatrix(), g_Engine.OBJECT_POS);
                glm::mat4 model = scene_model * mesh->getModelMatrix();

                shaderShadow->setMat4("model", model);

                mesh->draw();
            }
        }
    }
}


void sendOffscrUniforms(const Shader::Ptr& shader) {
    shader->setBool("deferred", false);

    shader->setMat4("view", g_View);
    shader->setMat4("projection", g_Proj);
    shader->setVec3("viewPos", camera::g_Camera.Position);
    shader->setBool("hasShadow", g_Engine.SHADOW_ENBL);

    shader->setMat4("lightSpaceMatrix", g_LightSpaceMatrix);

    shader->setInt("materialMaps.diffuse", TEXTURE_SLOT_DIFFUSE);
    shader->setInt("materialMaps.specular", TEXTURE_SLOT_SPECULAR);
    shader->setInt("materialMaps.shadow", TEXTURE_SLOT_SHADOW);
    shader->setInt("materialMaps.normal", TEXTURE_SLOT_NORMAL);
}

void sendOffscrPbrUniforms(const Shader::Ptr& shader) {
    shader->setBool("deferred", false);
    shader->setMat4("view", g_View);
    shader->setMat4("projection", g_Proj);
    shader->setVec3("camPos", camera::g_Camera.Position);

    shader->setInt("materialMaps.albedoMap", TEXTURE_SLOT_ALBEDO);
    shader->setInt("materialMaps.normal", TEXTURE_SLOT_NORMAL_PBR);
    shader->setInt("materialMaps.roughness", TEXTURE_SLOT_ROUGHNESS);
    shader->setInt("materialMaps.metallic", TEXTURE_SLOT_METALLIC);
    shader->setInt("materialMaps.ao", TEXTURE_SLOT_AO);
}

void sendLightPassUniforms(const Shader::Ptr& shader) {
    shader->setBool("deferred", true);

    shader->setVec3("viewPos", camera::g_Camera.Position);
    shader->setBool("hasShadow", g_Engine.SHADOW_ENBL);

    shader->setInt("materialMaps.diffuse", TEXTURE_SLOT_UNBOUND);
    shader->setInt("materialMaps.specular", TEXTURE_SLOT_UNBOUND);
    shader->setInt("materialMaps.normal", TEXTURE_SLOT_UNBOUND);

    shader->setInt("materialMaps.shadow", TEXTURE_SLOT_SHADOW);

    shader->setInt("deferredMaps.gPosition", TEXTURE_SLOT_DEFERRED_POSITION);
    shader->setInt("deferredMaps.gNormal", TEXTURE_SLOT_DEFERRED_NORMAL);
    shader->setInt("deferredMaps.gAlbedoSpec", TEXTURE_SLOT_DEFERRED_ALBEDOSPEC);
    shader->setInt("deferredMaps.gPositionLightSpace", TEXTURE_SLOT_DEFERRED_POSITION_LIGHT_SPACE);
}

void backBufferPass() {
    if (g_Engine.MSAA_ENBL)
        fboOffscrMSAA->bind();
    else
        fboOffscr->bind();

    glViewport(0, 0, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Models


    // necessary to set to black to avoid background influence on bloom pass
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Scene

    if (g_Engine.DEFERRED_SHADING) {
        shaderGLightPass->use();
        fboGBuffer->bindTextures();
        texShadowmap->bind();
        sendLightPassUniforms(shaderGLightPass);
        sendLightUniforms(shaderGLightPass);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenQuad->draw();

        fboGBuffer->blitDepthTo(fboOffscr, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
    } else {
        if (g_Engine.PBR_ENBL) {
            shaderPbr->use();
            sendOffscrPbrUniforms(shaderPbr);
            sendLightPbrUniforms(shaderPbr);
            renderScenes(shaderPbr);
        } else {
            shaderPhong->use();
            sendOffscrUniforms(shaderPhong);
            sendLightUniforms(shaderPhong);
            renderScenes(shaderPhong);
        }
    }

    // Draw skybox
    // Disable skybox influence on bloom bright buffer
    glColorMaski(1, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    shaderSkybox->use();
    shaderSkybox->setInt("skybox", TEXTURE_SLOT_SKYBOX);
    shaderSkybox->setMat4("view", glm::mat4(glm::mat3(g_View)));
    shaderSkybox->setMat4("projection", g_Proj);

    //skybox->draw();

    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Draw lights cube debug
    renderLightCubes(shaderDefault);

    if (g_Engine.MSAA_ENBL)
        fboOffscrMSAA->blitColorTo(fboOffscr, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void setupOffscrPass() {

    fboOffscr = FrameBuffer::New();

    texOffscr = ColorBufferTexture::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
    texOffscrBright = ColorBufferTexture::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    TextureConfig
        texOffscr_TConf = texOffscr->getTextureConfig(),
        texOffscrBright_TConf = texOffscrBright->getTextureConfig();

    texOffscr_TConf.hdr = texOffscrBright_TConf.hdr = g_Engine.HDR_ENBL;

    texOffscr->setTextureConfig(texOffscr_TConf);
    texOffscrBright->setTextureConfig(texOffscrBright_TConf);

    rboOffscr = RenderBuffer::New(RBType::DEPTH_STENCIL, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    fboOffscr->attachTexture(GL_COLOR_ATTACHMENT0, texOffscr);
    fboOffscr->attachTexture(GL_COLOR_ATTACHMENT1, texOffscrBright);
    fboOffscr->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, rboOffscr);

    fboOffscr->bind();
    fboOffscr->setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Offscr Framebuffer is not complete!" <<
            std::endl;

    fboOffscrMSAA = FrameBuffer::New();
    texOffscrMSAA = MultisampleTexture::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    TextureConfig texOffscrMSAA_TConf = texOffscrMSAA->getTextureConfig();
    texOffscrMSAA_TConf.msaa_multiplier = g_Engine.MSAA_MULTIPLIER;
    texOffscrMSAA->setTextureConfig(texOffscrMSAA_TConf);
    rboOffscrMSAA = RenderBuffer::New(RBType::DEPTH_STENCIL_MULTISAMPLE, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    fboOffscrMSAA->attachTexture(GL_COLOR_ATTACHMENT0, texOffscrMSAA);
    fboOffscrMSAA->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, rboOffscrMSAA);

    fboOffscrMSAA->bind();

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: MSAA Framebuffer is not complete!" <<
            std::endl;


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void shadowPass() {
    glViewport(0, 0, g_Engine.SHADOW_WIDTH, g_Engine.SHADOW_HEIGHT);

    glEnable(GL_DEPTH_TEST); // This single line took 3hrs of my life

    fboShadow->bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    bool shadowMapping = g_Engine.SHADOW_ENBL;
    shaderPhong->setBool("hasShadow", shadowMapping);
    shaderGLightPass->setBool("hasShadow", shadowMapping);

    if (!shadowMapping) return;

    shaderShadow->use();

    float near_plane = 1.0f, far_plane = 27.5f;
    glm::mat4 lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(
        -g_SunLight->getDirection(),
        glm::vec3(0.0f),
        glm::vec3(0.0, 1.0, 0.0)
    );

    g_LightSpaceMatrix = lightProj * lightView;


    shaderShadow->setMat4("lightSpaceMatrix", g_LightSpaceMatrix);

    // TODO: A mechanism to improve peter panning without removing 2d things

    //glCullFace(GL_FRONT);
    renderScenesDepth();
    //glCullFace(GL_BACK);

    fboShadow->unbind();
}

void setupShadowPass() {
    shaderShadow->use();

    fboShadow = FrameBuffer::New();
    fboShadow->bind();

    texShadowmap = DepthBufferTexture::New(g_Engine.SHADOW_WIDTH, g_Engine.SHADOW_HEIGHT);
    texShadowmap->setSlot(TEXTURE_SLOT_SHADOW);

    fboShadow->attachTexture(GL_DEPTH_ATTACHMENT, texShadowmap);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Shadow map Framebuffer is not complete!" <<
            std::endl;

    fboShadow->unbind();
}

void bloomPass() {

    if (!g_Engine.BLOOM_ENBL) return;

    shaderBlur->use();

    bool horizontal = true;
    unsigned int amount = 10;
    for (unsigned int i = 0; i < amount; i++)
    {
        if (horizontal)
            fboBlurHoriz->bind();
        else
            fboBlurVert->bind();

        shaderBlur->setInt("horizontal", horizontal);

        if (i == 0) // first iteration
            texOffscrBright->bind();
        else if (horizontal)
            texBlurHoriz->bind();
        else
            texBlurVert->bind();

        screenQuad->draw();

        horizontal = !horizontal;
    }
}

void setupBloomPass() {

    fboBlurHoriz = FrameBuffer::New();
    texBlurHoriz = ColorBufferTexture::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    fboBlurHoriz->attachTexture(GL_COLOR_ATTACHMENT0, texBlurHoriz);
    fboBlurHoriz->bind();

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Shadow map Framebuffer is not complete!" <<
            std::endl;

    fboBlurHoriz->unbind();

    fboBlurVert = FrameBuffer::New();
    texBlurVert = ColorBufferTexture::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    fboBlurVert->attachTexture(GL_COLOR_ATTACHMENT0, texBlurVert);
    fboBlurVert->bind();

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Shadow map Framebuffer is not complete!" <<
            std::endl;

    fboBlurVert->unbind();
}

void sendPostprocessUniforms() {
    shaderPostProcess->setInt("screenTexture", TEXTURE_SLOT_SCREEN);
    shaderPostProcess->setInt("bloomTexture", TEXTURE_SLOT_BLOOM);

    shaderPostProcess->setBool("gamma", true);
    shaderPostProcess->setBool("hdr", g_Engine.HDR_ENBL);
    shaderPostProcess->setBool("bloom", g_Engine.BLOOM_ENBL);
    shaderPostProcess->setFloat("exposure", g_Engine.HDR_EXPOSURE);

    shaderPostProcess->setBool("sharpen", g_Engine.SHARPNESS_ENBL);
    shaderPostProcess->setFloat("sharpness", g_Engine.SHARPNESS_AMOUNT);
    shaderPostProcess->setBool("blur", g_Engine.BLUR_ENBL);
    shaderPostProcess->setBool("grayscale", g_Engine.GRAYSCALE_ENBL);

    texOffscr->setSlot(TEXTURE_SLOT_SCREEN);
    texOffscr->bind();

    texBlurVert->setSlot(TEXTURE_SLOT_BLOOM);
    texBlurVert->bind();
}

void postprocessPass() {
    glViewport(0, 0, g_Engine.SCREEN_WIDTH, g_Engine.SCREEN_HEIGHT);

    shaderPostProcess->use();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sendPostprocessUniforms();

    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    screenQuad->draw();
}

void setupPostprocessPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    screenQuad = Quad::New();
}

void geometryPass() {
    fboGBuffer->bind();
    shaderGBuffer->use();

    glViewport(0, 0, g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Scene
    sendOffscrUniforms(shaderGBuffer);
    renderScenes(shaderGBuffer);

    // Draw skybox

    shaderSkybox->use();
    shaderSkybox->setInt("skybox", TEXTURE_SLOT_SKYBOX);
    shaderSkybox->setMat4("view", glm::mat4(glm::mat3(g_View)));
    shaderSkybox->setMat4("projection", g_Proj);

    fboGBuffer->unbind();
}

void setupGeometryPass() {
    fboGBuffer = GBuffer::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
}

void setupSSAOPass() {
    fboSSAO = FrameBuffer::New();
    texSSAO = MonoBufferTexture::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    fboSSAO->attachTexture(GL_COLOR_ATTACHMENT0, texSSAO);
    fboSSAO->bind();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    fboSSAOBlur = FrameBuffer::New();
    texSSAOBlur = MonoBufferTexture::New(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

    fboSSAOBlur->attachTexture(GL_COLOR_ATTACHMENT0, texSSAOBlur);
    fboSSAOBlur->bind();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = .1f + (scale * scale) * .9f;
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    TextureConfig tconf;
    tconf.min_filter = tconf.mag_filter = GL_NEAREST;
    tconf.wrap_s = tconf.wrap_t = GL_REPEAT;
    tconf.internal_format = GL_RGBA32F;
    tconf.data_format = GL_RGB;
    tconf.data_type = GL_FLOAT;

    texSSAONoise = Texture::New(
        &ssaoNoise[0],
        4, 4,
        TextureType::None,
        tconf
    );
}

void render() {

    using namespace window;

    // TODO: WHy dont yOu JusT not do tHis at all
    GLbitfield clr_enbl;

    glm::vec4& clr_color = g_Engine.CLEAR_COLOR;
    glClearColor(clr_color.r, clr_color.g, clr_color.b, clr_color.a);
    if (g_Engine.CLEAR_COLOR_BUF) clr_enbl |= GL_COLOR_BUFFER_BIT;
    if (g_Engine.CLEAR_DEPTH_BUF) clr_enbl |= GL_DEPTH_BUFFER_BIT;
    if (g_Engine.CLEAR_STENCIL_BUF) clr_enbl |= GL_STENCIL_BUFFER_BIT;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    g_View = camera::g_Camera.GetViewMatrix();
    g_Proj = glm::perspective(camera::g_Camera.Fov(),
                              ASPECT_RATIO, g_Engine.NEAR_PLANE, g_Engine.FAR_PLANE);

    // TODO: SSAO Pass
    shadowPass();
    if (g_Engine.DEFERRED_SHADING) geometryPass();
    backBufferPass();
    bloomPass();
    postprocessPass();
}

int init() {

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_STENCIL_TEST);
    //glEnable(GL_CULL_FACE);

    const auto SPath = [](const std::string p) -> const std::string {
        constexpr static std::string SHADER_DIR = "./shaders/";
        return SHADER_DIR + p;
    };

    shaderDefault = Shader::New(
        SPath("light_cube_vs.glsl"),
        SPath("light_cube_fs.glsl")
    );
    shaderPhong = Shader::New(
        SPath("phong_vs.glsl"),
        SPath("phong_fs.glsl")
    );
    shaderPostProcess = Shader::New(
        SPath("screen_PP_vs.glsl"),
        SPath("screen_PP_fs.glsl")
    );
    shaderSkybox = Shader::New(
        SPath("skybox_vs.glsl"),
        SPath("skybox_fs.glsl")
    );
    shaderShadow = Shader::New(
        SPath("shadow_map_vs.glsl"),
        SPath("shadow_map_fs.glsl")
    );
    shaderBlur = Shader::New(
        SPath("blur_vs.glsl"),
        SPath("blur_fs.glsl")
    );
    shaderGBuffer = Shader::New(
        SPath("g_buffer_vs.glsl"),
        SPath("g_buffer_fs.glsl")
    );
    shaderGLightPass = Shader::New(
        SPath("g_light_pass_vs.glsl"),
        SPath("phong_fs.glsl")
    );
    shaderPbr = Shader::New(
        SPath("pbr_vs.glsl"),
        SPath("pbr_fs.glsl")
    );

    Scene::Ptr scene = Scene::New();

    Model::Ptr model1 = Model::New("./assets/Sponza/glTF/Sponza.gltf");
    model1->scale(glm::vec3(0.01));
    Model::Ptr model2 = Model::New("./assets/backpack.obj");


    MeshGroup::Ptr test_shadow = MeshGroup::New();
    const Plane::Ptr plane = Plane::New();
    plane->rotate(90.f, glm::vec3(1.0f, 0.0, 0.0f));
    plane->translate(glm::vec3(0.0f, 0.0f, .5f));
    plane->scale(glm::vec3(20.f, 20.f, 1.f));
    const Cube::Ptr cb1 = Cube::New();
    const Cube::Ptr cb2 = Cube::New();
    const Cube::Ptr cb3 = Cube::New();

    cb1->translate(glm::vec3(0.0f, 1.5f, 0.0f));
    cb1->scale(glm::vec3(0.5));

    cb2->translate(glm::vec3(2.0f, 0.0f, 1.0f));
    cb2->scale(glm::vec3(0.5));

    cb3->translate(glm::vec3(-1.0f, 0.0f, 2.0f));
    cb3->rotate(60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, -1.0f)));
    cb3->scale(glm::vec3(0.25));


    test_shadow->addMesh(plane);
    test_shadow->addMesh(cb1);
    test_shadow->addMesh(cb2);
    test_shadow->addMesh(cb3);

    const Texture::Ptr wood_tex = Texture::New("./tex/wood.png");

    plane->addTexture(wood_tex);

    cb1->addTexture(wood_tex);
    cb2->addTexture(wood_tex);
    cb3->addTexture(wood_tex);

    MeshGroup::Ptr test_normal = MeshGroup::New();
    const Plane::Ptr planeN1 = Plane::New(), planeN2 = Plane::New();

    planeN2->translate(glm::vec3(2.0, 0.0, 0.0));
    const Texture::Ptr brick_tex = Texture::New("./tex/brickwall.jpg");
    const Texture::Ptr brick_tex_norm = Texture::New("./tex/brickwall_normal.jpg");

    brick_tex_norm->setType(TextureType::Normal);

    planeN1->addTexture(brick_tex);
    planeN1->addTexture(brick_tex_norm);
    planeN2->addTexture(brick_tex);

    test_normal->addMesh(planeN1);
    test_normal->addMesh(planeN2);

    MeshGroup::Ptr test_bloom = MeshGroup::New();

    Cube::Ptr floor_cube = Cube::New(),
    cube1 = Cube::New(), cube2 = Cube::New(), cube3 = Cube::New(), cube4 = Cube::New(), cube5 = Cube::New();

    floor_cube->translate(glm::vec3(0.0f, -1.0f, 0.0f));
    floor_cube->scale(glm::vec3(12.5f, 0.5f, 12.5f));

    cube1->translate(glm::vec3(0.0f, 1.5f, 0.0f));
    cube1->scale(glm::vec3(0.5f));

    cube2->translate(glm::vec3(2.0f, 0.0f, 1.0f));
    cube2->scale(glm::vec3(0.5f));

    cube3->translate(glm::vec3(-1.0f, -1.0f, 2.0f));
    cube3->rotate(60.0f, glm::vec3(1.0, 0.0, 0.0));

    cube4->translate(glm::vec3(-2.0f, 1.0f, -3.0f));
    cube4->rotate(124.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));

    cube5->translate(glm::vec3(-3.0f, 0.0f, 0.0f));
    cube5->scale(glm::vec3(0.5f));



    const Texture::Ptr container2_tex = Texture::New("./tex/container2.png");

    cube1->addTexture(container2_tex);
    cube2->addTexture(container2_tex);
    cube3->addTexture(container2_tex);
    cube4->addTexture(container2_tex);
    cube5->addTexture(container2_tex);

    floor_cube->addTexture(wood_tex);

    test_bloom->addMesh(floor_cube);
    test_bloom->addMesh(cube1);
    test_bloom->addMesh(cube2);
    test_bloom->addMesh(cube3);
    test_bloom->addMesh(cube4);
    test_bloom->addMesh(cube5);

    //addPointLight();
    //addPointLight();
    //addPointLight();
    //addPointLight();

    // lighting info
    // -------------
    // positions
    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f));
    lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
    lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
    // colors
    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
    lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
    lightColors.push_back(glm::vec3(0.0f,   0.0f,  15.0f));
    lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));

    //for (unsigned int i = 0; i < 4; i++) {
    //    PointLight::Ptr pl = std::dynamic_pointer_cast<PointLight, Light>(g_Lights[i]);
    //    pl->setPosition(lightPositions[i]);
    //    pl->setColor(lightColors[i]);
    //}

    MeshGroup::Ptr test_pbr = MeshGroup::New();

    glm::vec3 _albedo(0.5f, 0.0f, 0.0f);
    float _ao = 1.f;

    float spacing = 2.5f;
    for (int row = 0; row < 7; ++row)
    {
        float metallic = (float)row / 7;
        for (int col = 0; col < 7; ++col)
        {
            // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
            // on direct lighting.
            float roughness = glm::clamp((float)col / (float)7, 0.1f, 1.0f);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(
                (col - (7.f / 2)) * spacing,
                (row - (7.f / 2)) * spacing,
                0.0f
            ));
            Sphere::Ptr sphere = Sphere::New(64, 64);
            sphere->setModelMatrix(model);
            Material::Ptr mtl = PBRMaterial::New(_albedo, roughness, metallic, _ao);
            sphere->setMaterial(mtl);
            test_pbr->addMesh(sphere);
        }
    }
    // pbr test lights
    // ------
    glm::vec3 lightPos[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColor[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };

    for (unsigned int i = 0; i < 4; i++)
    {
        addPointLight(lightPos[i], lightColor[i]);
    }
    //scene->addGroup(test_bloom);
    //scene->addGroup(test_normal);
    //scene->addGroup(model1);
    //scene->addGroup(test_shadow);
    //scene->addGroup(model2);
    scene->addGroup(test_pbr);

    g_Scenes.push_back(scene);


    g_SunLight = DirectionalLight::New(
        g_Engine.LIGHT_DIR,
        g_Engine.LIGHT_AMBIENT,
        g_Engine.LIGHT_DIFFUSE,
        g_Engine.LIGHT_SPECULAR
    );

    pointLightsCube = Cube::New();
    spotLightsCube = Cube::New();

    // TODO: this is very ugly

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

    skybox = Skybox::New(faces);

    std::cout << "BEFORE SHADOW SETUP" << std::endl;
    setupShadowPass();
    std::cout << "AFTER SHADOW SETUP" << std::endl;
    setupOffscrPass();
    setupGeometryPass();
    setupBloomPass();
    setupSSAOPass();
    setupPostprocessPass();

    return 0;
}

void updateState() {
    camera::g_Camera.updateCamera(camera::CAMERA_STATE);

    g_Engine.CLEAR_COLOR = ENGINE_STATE.CLEAR_COLOR;

    // model
    if (g_Engine.OBJECT_POS != ENGINE_STATE.OBJECT_POS)
        g_Engine.OBJECT_POS = ENGINE_STATE.OBJECT_POS;

    // lighting
    if (g_Engine.LIGHT_DIR != ENGINE_STATE.LIGHT_DIR) {
        g_Engine.LIGHT_DIR = ENGINE_STATE.LIGHT_DIR;
        g_SunLight->setDirection(g_Engine.LIGHT_DIR);
    }
    if (g_Engine.LIGHT_AMBIENT != ENGINE_STATE.LIGHT_AMBIENT) {
        g_Engine.LIGHT_AMBIENT = ENGINE_STATE.LIGHT_AMBIENT;
        g_SunLight->setAmbient(g_Engine.LIGHT_AMBIENT);
    }

    if (g_Engine.LIGHT_DIFFUSE != ENGINE_STATE.LIGHT_DIFFUSE) {
        g_Engine.LIGHT_DIFFUSE = ENGINE_STATE.LIGHT_DIFFUSE;
        g_SunLight->setDiffuse(g_Engine.LIGHT_DIFFUSE);
    }

    if (g_Engine.LIGHT_SPECULAR != ENGINE_STATE.LIGHT_SPECULAR) {
        g_Engine.LIGHT_SPECULAR = ENGINE_STATE.LIGHT_SPECULAR;
        g_SunLight->setSpecular(g_Engine.LIGHT_SPECULAR);
    }

    if (g_Engine.BLINN_ENBL != ENGINE_STATE.BLINN_ENBL)
        g_Engine.BLINN_ENBL = ENGINE_STATE.BLINN_ENBL;

    // postprocess
    if (g_Engine.SHARPNESS_ENBL != ENGINE_STATE.SHARPNESS_ENBL)
        g_Engine.SHARPNESS_ENBL = ENGINE_STATE.SHARPNESS_ENBL;
    if (g_Engine.SHARPNESS_AMOUNT != ENGINE_STATE.SHARPNESS_AMOUNT)
        g_Engine.SHARPNESS_AMOUNT = ENGINE_STATE.SHARPNESS_AMOUNT;
    if (g_Engine.BLUR_ENBL != ENGINE_STATE.BLUR_ENBL)
        g_Engine.BLUR_ENBL = ENGINE_STATE.BLUR_ENBL ;
    if (g_Engine.GRAYSCALE_ENBL != ENGINE_STATE.GRAYSCALE_ENBL)
        g_Engine.GRAYSCALE_ENBL = ENGINE_STATE.GRAYSCALE_ENBL;

    if (g_Engine.SHADOW_ENBL != ENGINE_STATE.SHADOW_ENBL)
        g_Engine.SHADOW_ENBL = ENGINE_STATE.SHADOW_ENBL;

    if (g_Engine.SHADOW_ENBL && (g_Engine.SHADOW_WIDTH != ENGINE_STATE.SHADOW_WIDTH)) {

        g_Engine.SHADOW_WIDTH = ENGINE_STATE.SHADOW_WIDTH;
        g_Engine.SHADOW_HEIGHT = ENGINE_STATE.SHADOW_HEIGHT;

        texShadowmap->resize(g_Engine.SHADOW_WIDTH, g_Engine.SHADOW_HEIGHT);
    }

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

    if (g_Engine.HDR_ENBL != ENGINE_STATE.HDR_ENBL) {
        g_Engine.HDR_ENBL = ENGINE_STATE.HDR_ENBL;
        regen_buffers = true;
    }

    if (g_Engine.BLOOM_ENBL != ENGINE_STATE.BLOOM_ENBL) {
        g_Engine.BLOOM_ENBL = ENGINE_STATE.BLOOM_ENBL;
        regen_buffers = true;
    }

    if (g_Engine.HDR_ENBL && (g_Engine.HDR_EXPOSURE != ENGINE_STATE.HDR_EXPOSURE))
        g_Engine.HDR_EXPOSURE = ENGINE_STATE.HDR_EXPOSURE;

    if (g_Engine.DEFERRED_SHADING != ENGINE_STATE.DEFERRED_SHADING) {
        g_Engine.DEFERRED_SHADING = ENGINE_STATE.DEFERRED_SHADING;
        regen_buffers = true;
    }

    if (regen_buffers) {

        TextureConfig
            texOffscr_TConf = texOffscr->getTextureConfig(),
            texOffscrBright_TConf = texOffscrBright->getTextureConfig(),
            texOffscrMSAA_TConf = texOffscrMSAA->getTextureConfig();

        texOffscr_TConf.hdr = texOffscrBright_TConf.hdr = texOffscrMSAA_TConf.hdr
            = g_Engine.HDR_ENBL;

        texOffscrMSAA_TConf.msaa_multiplier = g_Engine.MSAA_MULTIPLIER;

        texOffscr->setTextureConfig(texOffscr_TConf);
        texOffscrBright->setTextureConfig(texOffscrBright_TConf);
        texOffscrMSAA->setTextureConfig(texOffscrMSAA_TConf);

        texOffscrMSAA->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
        texOffscrBright->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
        rboOffscrMSAA->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

        texOffscr->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
        rboOffscr->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

        texBlurHoriz->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
        texBlurVert->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);

        fboGBuffer->resize(g_Engine.RENDER_WIDTH, g_Engine.RENDER_HEIGHT);
    }

    if (g_Engine.SCREEN_WIDTH != ENGINE_STATE.SCREEN_WIDTH) {

        g_Engine.SCREEN_WIDTH = ENGINE_STATE.SCREEN_WIDTH;
        g_Engine.SCREEN_HEIGHT = ENGINE_STATE.SCREEN_HEIGHT;

        window::resize_window(g_Engine.SCREEN_WIDTH, g_Engine.SCREEN_HEIGHT);
    }

    ENGINE_STATE = g_Engine;
}


void addSpotLight() {
    constexpr glm::vec3 initial_pos(-8.0, 2.0, 3.0);
    constexpr glm::vec3 initial_dir(0.0, 1.0, 0.0);
    constexpr glm::vec3 initial_color(1.0);
    constexpr unsigned int initial_distance = 30;

    if (g_Lights.size() < renderer::NR_MAX_LIGHTS)
        g_Lights.push_back(SpotLight::New(initial_pos, initial_dir, initial_distance, initial_color, 10.f, 15.f));
}

void addPointLight() {
    constexpr glm::vec3 initial_pos(2.0, 2.0, 5.0);
    constexpr glm::vec3 initial_color(1.0);
    constexpr unsigned int initial_distance = 13;

    if (g_Lights.size() < renderer::NR_MAX_LIGHTS)
        g_Lights.push_back(PointLight::New(initial_pos, initial_distance, initial_color));
}

void addPointLight(glm::vec3 position, glm::vec3 color) {
    constexpr unsigned int initial_distance = 13;
    if (g_Lights.size() < renderer::NR_MAX_LIGHTS)
        g_Lights.push_back(PointLight::New(position, initial_distance, color));
}

size_t getLightsCount(LightType lt) {
    size_t count = 0;
    for (const auto& light : g_Lights)
    if (light->getType() == lt)
        count++;
    return count;
}

void removeLight(int index) {
    g_Lights.erase(g_Lights.begin() + index);
}

const Light::Ptr getLight(int index) {
    return
    index < g_Lights.size() ?
    g_Lights[index] :
    nullptr;
}


void terminate() {}
}
