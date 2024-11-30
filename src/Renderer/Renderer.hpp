#ifndef RENDERER_H
#define RENDERER_H

#include "Core/FrameBuffer.hpp"
#include "Core/GBuffer.hpp"
#include "Core/RenderBuffer.hpp"
#include "Core/Scene.hpp"
#include "Core/Shapes/Cube.hpp"
#include "Core/Shapes/Quad.hpp"
#include "Renderer/Skybox.hpp"
#include "Lighting/Light.hpp"
#include "Lighting/DirectionalLight.hpp"
#include "Camera.hpp"
#include "Texture/CubeMapBufferTexture.hpp"
#include "Texture/MonoBufferTexture.hpp"
#include "Texture/MultisampleTexture.hpp"
#include "Texture/ColorBufferTexture.hpp"
#include "Texture/DepthBufferTexture.hpp"

#include <concepts>
#include <cstdint>

namespace renderer
{
struct EngineState {

    glm::vec3 LIGHT_DIR;
    glm::vec3 LIGHT_AMBIENT;
    glm::vec3 LIGHT_DIFFUSE;
    glm::vec3 LIGHT_SPECULAR;

    int BLINN_ENBL;

    glm::vec3 OBJECT_POS;
    glm::vec3 OBJECT_ROTATION;
    float OBJ_SCALE;

    unsigned int SHARPNESS_ENBL;
    float SHARPNESS_AMOUNT;

    unsigned int BLUR_ENBL;
    unsigned int GRAYSCALE_ENBL;

    bool CLEAR_DEPTH_BUF, CLEAR_COLOR_BUF, CLEAR_STENCIL_BUF;

    glm::vec4 CLEAR_COLOR;

    float NEAR_PLANE, FAR_PLANE, FOV;

    uint32_t RENDER_WIDTH, RENDER_HEIGHT;
    uint32_t SCREEN_WIDTH, SCREEN_HEIGHT;

    unsigned int MSAA_ENBL;
    unsigned int MSAA_MULTIPLIER;

    int SHADOW_ENBL;
    unsigned int SHADOW_WIDTH;
    unsigned int SHADOW_HEIGHT;

    int HDR_ENBL;
    float HDR_EXPOSURE;

    int BLOOM_ENBL;
    int DEFERRED_SHADING;
    int PBR_ENBL;

    EngineState() {
        LIGHT_DIR = glm::vec3(-4.0f, -2.0f, -3.0f);
        LIGHT_AMBIENT = glm::vec3(0.1f);
        LIGHT_DIFFUSE = glm::vec3(0.5f);
        LIGHT_SPECULAR = glm::vec3(1.0f);

        BLINN_ENBL = false;

        OBJECT_POS = glm::vec3(0.0f);
        OBJECT_ROTATION = glm::vec3(0.0f);
        OBJ_SCALE = 1.0;

        SHARPNESS_ENBL = false;
        SHARPNESS_AMOUNT = 1.0f;

        BLUR_ENBL = false;
        GRAYSCALE_ENBL = false;

        CLEAR_COLOR_BUF = CLEAR_DEPTH_BUF =
            CLEAR_STENCIL_BUF = true;

        CLEAR_COLOR = glm::vec4(0.2f, 0.1f, 0.15f, 1.0f);

        NEAR_PLANE = 0.1f;
        FAR_PLANE = 100.0f;

        RENDER_WIDTH = SCREEN_WIDTH = 1280;
        RENDER_HEIGHT = SCREEN_HEIGHT = 720;

        // Apple has problems with MSAA
        #ifdef __APPLE__
            MSAA_ENBL = false;
        #else
            MSAA_ENBL = false;
        #endif

        MSAA_MULTIPLIER = 4;

        SHADOW_ENBL = true;
        SHADOW_WIDTH = SHADOW_HEIGHT = 1024;

        HDR_ENBL = true;
        HDR_EXPOSURE = 1.f;

        BLOOM_ENBL = false;

        DEFERRED_SHADING = false;

        PBR_ENBL = true;
    }
};

// Texture slots for shaderPhong
constexpr static unsigned int TEXTURE_SLOT_DIFFUSE = 0;
constexpr static unsigned int TEXTURE_SLOT_SPECULAR = 1;
constexpr static unsigned int TEXTURE_SLOT_SHADOW = 2;
constexpr static unsigned int TEXTURE_SLOT_NORMAL = 3;

constexpr static unsigned int TEXTURE_SLOT_DEFERRED_POSITION = 5;
constexpr static unsigned int TEXTURE_SLOT_DEFERRED_NORMAL = 6;
constexpr static unsigned int TEXTURE_SLOT_DEFERRED_ALBEDOSPEC = 7;
constexpr static unsigned int TEXTURE_SLOT_DEFERRED_POSITION_LIGHT_SPACE = 8;

constexpr static unsigned int TEXTURE_SLOT_UNBOUND = 15;

// Texture slots for shaderPbr
constexpr static unsigned int TEXTURE_SLOT_ALBEDO = 0;
constexpr static unsigned int TEXTURE_SLOT_METALLIC = 1;
constexpr static unsigned int TEXTURE_SLOT_ROUGHNESS = 2;
constexpr static unsigned int TEXTURE_SLOT_NORMAL_PBR = TEXTURE_SLOT_NORMAL;
constexpr static unsigned int TEXTURE_SLOT_AO = 4;

// Texture slots for shaderPostProcess
constexpr static unsigned int TEXTURE_SLOT_SCREEN = 0;
constexpr static unsigned int TEXTURE_SLOT_BLOOM = 1;

// Texture slots for shaderSkybox
constexpr static unsigned int TEXTURE_SLOT_SKYBOX = 0;

constexpr static float ASPECT_RATIO = 16.0 / 9.0;
constexpr static unsigned int NR_MAX_LIGHTS = 10;

namespace camera {
    extern Camera CAMERA_STATE;
    extern Camera g_Camera;
}

extern EngineState ENGINE_STATE;
extern EngineState g_Engine;

extern Shader::Ptr shaderDefault;
extern Shader::Ptr shaderPhong;
extern Shader::Ptr shaderPostProcess;
extern Shader::Ptr shaderSkybox;
extern Shader::Ptr shaderShadow;
extern Shader::Ptr shaderBlur;
extern Shader::Ptr shaderGBuffer;
extern Shader::Ptr shaderGLightPass;
extern Shader::Ptr shaderPbr;
extern Shader::Ptr shaderEquirectangularToCubemap;

extern std::vector<Scene::Ptr> g_Scenes;
extern DirectionalLight::Ptr g_SunLight;
extern std::vector<Light::Ptr> g_Lights;

extern glm::mat4 g_View;
extern glm::mat4 g_Proj;
extern glm::mat4 g_LightSpaceMatrix;

extern FrameBuffer::Ptr fboShadow;
extern FrameBuffer::Ptr fboOffscrMSAA;
extern FrameBuffer::Ptr fboOffscr;
extern FrameBuffer::Ptr fboBlurHoriz;
extern FrameBuffer::Ptr fboBlurVert;
extern FrameBuffer::Ptr fboSSAO;
extern FrameBuffer::Ptr fboSSAOBlur;
extern FrameBuffer::Ptr fboEquirectangular;

extern GBuffer::Ptr fboGBuffer;

extern RenderBuffer::Ptr rboOffscr;
extern RenderBuffer::Ptr rboOffscrMSAA;
extern RenderBuffer::Ptr rboEquirectangular;

extern DepthBufferTexture::Ptr texShadowmap;
extern ColorBufferTexture::Ptr texOffscr;
extern ColorBufferTexture::Ptr texOffscrBright;
extern MultisampleTexture::Ptr texOffscrMSAA;
extern ColorBufferTexture::Ptr texBlurHoriz;
extern ColorBufferTexture::Ptr texBlurVert;
extern MonoBufferTexture::Ptr texSSAO;
extern MonoBufferTexture::Ptr texSSAOBlur;
extern Texture::Ptr texSSAONoise;
extern CubeMapBufferTexture::Ptr texEnvironmentMap;

extern Quad::Ptr screenQuad;
extern Cube::Ptr pointLightsCube;

// TODO: make me a prism or something
extern Cube::Ptr spotLightsCube;


extern Skybox::Ptr skybox;
// proj and view
// camera
// vector lights
// fbos
// quad vao and vbo
// skybox
//
int init();
void updateState();
void render();
void terminate();

size_t getLightsCount(LightType lt);
void addSpotLight();
void addPointLight();
void addPointLight(glm::vec3 position, glm::vec3 color = glm::vec3(1.f));
void removeLight(int index);
const Light::Ptr getLight(int index);

}

#endif
