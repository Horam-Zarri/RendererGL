#include "gui.hpp"
#include "imgui.h"
#include "renderer.hpp"

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


    using namespace renderer;

    if (ImGui::TreeNode("Model")) {

        ImGui::SeparatorText("Position:");

        ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2);
        ImGui::DragFloat("X", &x); ImGui::SameLine();
        ImGui::DragFloat("Y", &x); ImGui::SameLine();
        ImGui::DragFloat("Z", &x);

        ImGui::PopItemWidth();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Camera")) {

        ImGui::SeparatorText("Position: ");

        ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2);

        ImGui::DragFloat("X", &camera::CAMERA_STATE.Position.x); ImGui::SameLine();
        ImGui::DragFloat("Y", &camera::CAMERA_STATE.Position.y); ImGui::SameLine();
        ImGui::DragFloat("Z", &camera::CAMERA_STATE.Position.z);

        ImGui::Spacing();

        ImGui::SeparatorText("Rotation: ");

        ImGui::DragFloat("Yaw", &camera::CAMERA_STATE.Yaw); ImGui::SameLine();
        ImGui::DragFloat("Pitch", &camera::CAMERA_STATE.Pitch); ImGui::SameLine();


        ImGui::PopItemWidth();

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
