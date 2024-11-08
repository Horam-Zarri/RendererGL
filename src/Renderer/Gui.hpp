#ifndef GUI_H
#define GUI_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace GUI
{
    void setup(GLFWwindow* window);
    void render();
}
#endif
