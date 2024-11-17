#include "Gui.hpp"
#include "Lighting/Light.hpp"
#include "Lighting/PointLight.hpp"
#include "Lighting/SpotLight.hpp"

#include "imgui.h"
#include "Renderer.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

void settings_panel() {


    ImGuiViewport* window_full = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(window_full->Pos.x + window_full->Size.x * 0.7, window_full->Pos.y));
    ImGui::SetNextWindowSize(ImVec2(window_full->Size.x * 0.3, window_full->Size.y));

    ImGuiWindowFlags flags = 0;

    flags |= ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize;

    if (!ImGui::Begin("Work Space", NULL, flags))
    {
        ImGui::End();
        return;
    }


    using renderer::ENGINE_STATE;
    using renderer::camera::CAMERA_STATE;


    if (ImGui::TreeNode("Model")) {

        ImGui::SeparatorText("Position:");

        ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2);
        ImGui::DragFloat("X", &ENGINE_STATE.OBJECT_POS.x); ImGui::SameLine();
        ImGui::DragFloat("Y", &ENGINE_STATE.OBJECT_POS.y); ImGui::SameLine();
        ImGui::DragFloat("Z", &ENGINE_STATE.OBJECT_POS.z);

        ImGui::PopItemWidth();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Camera")) {

        ImGui::SeparatorText("Position: ");

        ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2);

        ImGui::DragFloat("X", &CAMERA_STATE.Position.x); ImGui::SameLine();
        ImGui::DragFloat("Y", &CAMERA_STATE.Position.y); ImGui::SameLine();
        ImGui::DragFloat("Z", &CAMERA_STATE.Position.z);

        ImGui::Spacing();

        ImGui::SeparatorText("Rotation: ");

        ImGui::DragFloat("Yaw", &CAMERA_STATE.Yaw); ImGui::SameLine();
        ImGui::DragFloat("Pitch", &CAMERA_STATE.Pitch); ImGui::SameLine();


        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Lighting")) {

        ImGui::SeparatorText("General");

        ImGui::Checkbox("Blinn", (bool*)&ENGINE_STATE.BLINN_ENBL);

        ImGui::Checkbox("HDR", (bool*)&ENGINE_STATE.HDR_ENBL);

        if (ENGINE_STATE.HDR_ENBL)
            ImGui::SliderFloat("Exposure", &ENGINE_STATE.HDR_EXPOSURE, 0.f, 5.f);

        ImGui::Checkbox("Bloom", (bool*)&ENGINE_STATE.BLOOM_ENBL);


        ImGui::SeparatorText("Shadows");

        static int shadow_state = 1;

        const char* shadow_options[] = {
            "None",
            "Low",
            "Medium",
            "High",
            "Ultra"
        };

        if (ImGui::Combo(
            "Shadow mapping",
            &shadow_state,
            shadow_options,
            IM_ARRAYSIZE(shadow_options)
        )) {
            ENGINE_STATE.SHADOW_ENBL = shadow_state != 0;
            if (ENGINE_STATE.SHADOW_ENBL) {
                ENGINE_STATE.SHADOW_WIDTH = ENGINE_STATE.SHADOW_HEIGHT
                    = std::pow(2, shadow_state) * 1024;
            }
        }


        ImGui::SeparatorText("Directional Sun Light");

        ImGui::Text("Direction:");

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);

        ImGui::DragFloat("X", &ENGINE_STATE.LIGHT_DIR.x); ImGui::SameLine();
        ImGui::DragFloat("Y", &ENGINE_STATE.LIGHT_DIR.y); ImGui::SameLine();
        ImGui::DragFloat("Z", &ENGINE_STATE.LIGHT_DIR.z);

        ImGui::PopItemWidth();

        ImGui::DragFloat3("Ambient", &ENGINE_STATE.LIGHT_AMBIENT.x, .05f, 0.0f);
        ImGui::DragFloat3("Diffuse", &ENGINE_STATE.LIGHT_DIFFUSE.x, .05f, 0.0f);
        ImGui::DragFloat3("Specular", &ENGINE_STATE.LIGHT_SPECULAR.x, .05f, 0.0f);

        ImGui::SeparatorText("Scene Lights");

        if (ImGui::Button("Add point light")) {
            renderer::addPointLight();
        }
        ImGui::SameLine();
        if (ImGui::Button("Add spot light")) {
            renderer::addSpotLight();
        }

        ImGui::Spacing();

        // TODO: eh?
        for (unsigned int i = 0; i < renderer::NR_MAX_LIGHTS; i++) {
            auto scene_light = renderer::getLight(i);

            if (scene_light == nullptr)
                break;

            ImGui::PushID(i);

            auto light_type = scene_light->getType();

            ImGui::Text("Scene Light - %d", i);

            ImGui::SameLine();
            if (ImGui::Button("Remove")) {
                renderer::removeLight(i);
            }

            auto pl = std::dynamic_pointer_cast<PointLight, Light>(scene_light);

            if (pl == nullptr) continue;

            auto sl = std::dynamic_pointer_cast<SpotLight, PointLight>(pl);

            auto pl_pos = pl->getPosition();
            auto pl_ambient = pl->getAmbient();
            auto pl_diffuse = pl->getDiffuse();
            auto pl_specular = pl->getSpecular();
            auto pl_atten = pl->getAttenuation();

            if (ImGui::DragFloat3("Position", &pl_pos.x)) { pl->setPosition(pl_pos); }

            if (sl != nullptr)  {
                auto sl_dir = sl->getDirection();
                if (ImGui::DragFloat3("Direction", &sl_dir.x)) { sl->setDirection(sl_dir); }
            }

            if (ImGui::DragFloat3("Ambient", &pl_ambient.x, .05f, 0.0f)) { pl->setAmbient(pl_ambient); }
            if (ImGui::DragFloat3("Diffuse", &pl_diffuse.x, .05f, 0.0f)) { pl->setDiffuse(pl_diffuse); }
            if (ImGui::DragFloat3("Specular", &pl_specular.x, .05f, 0.0f)) { pl->setSpecular(pl_specular); }

            ImGui::Spacing();
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2);

            if (ImGui::DragFloat("Constant", &pl_atten.constant, .01f, 1.0)) { pl->setAttenuation(pl_atten); }
            ImGui::SameLine();
            if (ImGui::DragFloat("Linear", &pl_atten.linear, .01f, 0.0)) { pl->setAttenuation(pl_atten); }
            if (ImGui::DragFloat("Quadratic", &pl_atten.quadratic, .01, 0.0)) { pl->setAttenuation(pl_atten); }


            if (sl == nullptr) {
                // necessary to sync imgui queues
                ImGui::PopItemWidth();
                ImGui::PopID();
                continue;
            }

            auto sl_cut_off = sl->getCutOffDeg();
            auto sl_outer_cut_off = sl->getOuterCutOffDeg();

            if (ImGui::DragFloat("CutOff Degrees", &sl_cut_off)) { sl->setCutOff(sl_cut_off); }
            if (ImGui::DragFloat("Outer CutOff Degrees", &sl_outer_cut_off)) { sl->setOuterCutOff(sl_outer_cut_off); }

            ImGui::PopItemWidth();

            ImGui::PopID();
        }

        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Rendering")) {

        ImGui::SeparatorText("General");

        // default res_options[2] = 1280x720
        static int render_res_state = 5;
        static int window_res_state = 5;

        const char* res_options[] = {
            "640x360",
            "854x480",
            "960x540",
            "1024x576",
            "1152x648",
            "1280x720",
            "1366x768",
            "1600x900",
            "1920x1080",
            "2560x1440",
            "3840x2160"
        };

        struct res_t {
            unsigned int width;
            unsigned int height;
        };

        auto str_to_res = [=](std::string rs) -> res_t {
            unsigned int res_w = std::stoi(rs.substr(0, rs.find('x')));
            unsigned int res_h = std::stoi(rs.substr(rs.find('x') + 1));

            return res_t {.width = res_w, .height = res_h};
        };

        if (ImGui::Combo(
            "Render Res",
            &render_res_state,
            res_options,
            IM_ARRAYSIZE(res_options)
        )) {
            res_t r = str_to_res(res_options[render_res_state]);

            ENGINE_STATE.RENDER_WIDTH = r.width;
            ENGINE_STATE.RENDER_HEIGHT = r.height;
        }

        if (ImGui::Combo(
            "Window Res",
            &window_res_state,
            res_options,
            IM_ARRAYSIZE(res_options)
        )) {
            res_t r = str_to_res(res_options[window_res_state]);

            ENGINE_STATE.SCREEN_WIDTH = r.width;
            ENGINE_STATE.SCREEN_HEIGHT = r.height;
        }

        const char* antialiasing_options[] = {
            "Disabled",
            "MSAA x2",
            "MSAA x4",
            "MSAA x8",
        };

        // not elegant, yet so robust
        int can_enable_aa = !(ENGINE_STATE.HDR_ENBL || ENGINE_STATE.BLOOM_ENBL || ENGINE_STATE.DEFERRED_SHADING);
        int aa_state = ENGINE_STATE.MSAA_ENBL ? std::log2(ENGINE_STATE.MSAA_MULTIPLIER): 0;

        if (!can_enable_aa) {
            aa_state = ENGINE_STATE.MSAA_ENBL = false;
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
            ImGui::Text("MSAA cant be enabled with hdr | bloom | deferred");
            ImGui::PopStyleColor();
            ImGui::BeginDisabled();
        }

        if (ImGui::Combo(
            "Antialiasing",
            &aa_state,
            antialiasing_options,
            IM_ARRAYSIZE(antialiasing_options)
        )) {
            ENGINE_STATE.MSAA_ENBL = aa_state == 0 ? false : true;
            ENGINE_STATE.MSAA_MULTIPLIER = std::pow(2, aa_state);
        }

        if (!can_enable_aa)
            ImGui::EndDisabled();

        ImGui::ColorEdit4("Clear color", &ENGINE_STATE.CLEAR_COLOR.x);

        ImGui::SeparatorText("Postprocessing");

        static int post_state = 0;

        ImGui::RadioButton("None", &post_state, 0); ImGui::SameLine();
        ImGui::RadioButton("Sharpness", &post_state, 1); ImGui::SameLine();
        ImGui::RadioButton("Blur", &post_state, 2); ImGui::SameLine();
        ImGui::RadioButton("Grayscale", &post_state, 3);

        // TODO: This is pretty ugly and there could be edge cases
        switch (post_state) {
            case 0:
                ENGINE_STATE.SHARPNESS_ENBL = false;
                ENGINE_STATE.BLUR_ENBL = false;
                ENGINE_STATE.GRAYSCALE_ENBL = false;
                break;
            case 1:
                ENGINE_STATE.SHARPNESS_ENBL = true;
                ENGINE_STATE.BLUR_ENBL = false;
                ENGINE_STATE.GRAYSCALE_ENBL = false;

                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
                ImGui::SliderFloat("Sharpness amount", &ENGINE_STATE.SHARPNESS_AMOUNT, 0.0f, 1.0f);
                ImGui::PopItemWidth();
                break;
            case 2:
                ENGINE_STATE.BLUR_ENBL = true;
                ENGINE_STATE.SHARPNESS_ENBL = false;
                ENGINE_STATE.GRAYSCALE_ENBL = false;
                break;
            case 3:
                ENGINE_STATE.GRAYSCALE_ENBL = true;
                ENGINE_STATE.SHARPNESS_ENBL = false;
                ENGINE_STATE.BLUR_ENBL = false;
                break;
            default:
                break;
        }


        ImGui::TreePop();
    }

    ImGui::End();
}


void GUI::setup(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void GUI::render() {
    settings_panel();
}
