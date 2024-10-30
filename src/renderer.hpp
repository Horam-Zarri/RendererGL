#ifndef RENDERER_H
#define RENDERER_H

#include "camera.hpp"
#include <cstdint>

namespace renderer
{
struct EngineState {
    glm::vec3 LIGHT_POS;
    glm::vec3 LIGHT_AMBIENT;
    glm::vec3 LIGHT_DIFFUSE;
    glm::vec3 LIGHT_SPECULAR;

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

    EngineState() {
        LIGHT_POS = glm::vec3(4.0f, 2.0f, 3.0f);
        LIGHT_AMBIENT = glm::vec3(0.1f);
        LIGHT_DIFFUSE = glm::vec3(0.5f);
        LIGHT_SPECULAR = glm::vec3(1.0f);

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

        MSAA_ENBL = true;
        MSAA_MULTIPLIER = 4;
    }
};

namespace camera {
    extern Camera CAMERA_STATE;
    extern Camera g_Camera;
}

extern EngineState ENGINE_STATE;
extern EngineState g_Engine;

constexpr static float ASPECT_RATIO = 16.0 / 9.0;

int init();
void update_state();
void render();
void terminate();


}

#endif
