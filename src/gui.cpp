#include "gui.hpp"
#include "imgui.h"
#include "renderer.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

float x = 50;

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


    if (ImGui::TreeNode("Rendering")) {

        ImGui::SeparatorText("General");

        // default res_options[2] = 1280x720
        static int selected_res = 2;

        const char* res_options[] = {
            "1024x576",
            "1152x648",
            "1280x720",
            "1366x768",
            "1600x900",
            "1920x1080",
            "2560x1440",
            "3840x2160"
        };

        if (ImGui::Combo("Render Res", &selected_res, res_options, IM_ARRAYSIZE(res_options))) {
            std::string rs = res_options[selected_res];

            unsigned int res_w = std::stoi(rs.substr(0, rs.find('x')));
            unsigned int res_h = std::stoi(rs.substr(rs.find('x') + 1));

            ENGINE_STATE.RENDER_WIDTH = res_w;
            ENGINE_STATE.RENDER_HEIGHT = res_h;
        }

        ImGui::ColorEdit4("Clear color", &ENGINE_STATE.CLEAR_COLOR.x);

        ImGui::SeparatorText("Lighting");

        ImGui::Text("Position:");

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);

        ImGui::DragFloat("X", &ENGINE_STATE.LIGHT_POS.x); ImGui::SameLine();
        ImGui::DragFloat("Y", &ENGINE_STATE.LIGHT_POS.y); ImGui::SameLine();
        ImGui::DragFloat("Z", &ENGINE_STATE.LIGHT_POS.z);

        ImGui::PopItemWidth();

        ImGui::DragFloat3("Ambient", &ENGINE_STATE.LIGHT_AMBIENT.x, .05f, 0.0f, 1.0f);
        ImGui::DragFloat3("Diffuse", &ENGINE_STATE.LIGHT_DIFFUSE.x, .05f, 0.0f, 1.0f);
        ImGui::DragFloat3("Specular", &ENGINE_STATE.LIGHT_SPECULAR.x, .05f, 0.0f, 1.0f);


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
