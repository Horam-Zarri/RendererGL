#include "gui.hpp"
#include "imgui.h"
#include "renderer.hpp"
#include <iostream>

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

        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Rendering")) {

        ImGui::SeparatorText("General");

        ImGui::ColorEdit4("Clear color", &ENGINE_STATE.CLEAR_COLOR.x);

        ImGui::SeparatorText("Postprocessing");

        static int post_state = 0;

        ImGui::RadioButton("Sharpness", &post_state, 0); ImGui::SameLine();
        ImGui::RadioButton("Blur", &post_state, 1); ImGui::SameLine();
        ImGui::RadioButton("Grayscale", &post_state, 2);

        // TODO: Implement actual postprocessing settings
        switch (post_state) {
            case 0:
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
                ImGui::SliderFloat("Sharpness amount", &x, 0.0f, 1.0f);
                ImGui::PopItemWidth();
            break;
            case 2:
                ImGui::SliderFloat3("Weights", &x, 0.0f, 1.0f);
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
