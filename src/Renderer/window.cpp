#include "Window.hpp"
#include "Callbacks.hpp"

#include <iostream>


int window::init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif

    // glfw window creation
    // --------------------
    _Window = glfwCreateWindow(window::WINDOW_WIDTH, window::WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (_Window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    using namespace window;

    glfwMakeContextCurrent(_Window);
    glfwSetFramebufferSizeCallback(_Window, callback::framebuffer_size_callback);

    glfwSetCursorPosCallback(_Window, callback::mouse_callback);
    glfwSetScrollCallback(_Window, callback::scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GUI::setup(_Window);

    return 0;
}

bool window::should_terminate() {
    return glfwWindowShouldClose(_Window);
}

void window::resize_window(int width, int height) {
    glfwSetWindowSize(_Window, width, height);
}

void window::handle_input() {
    float current_frame = static_cast<float>(glfwGetTime());
    delta_frame = current_frame - last_frame;
    last_frame = current_frame;

    window::callback::process_input(_Window, delta_frame);
}


void window::render_gui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    GUI::render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void window::swap_and_poll() {
    glfwSwapBuffers(_Window);
    glfwPollEvents();
}


void window::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}
