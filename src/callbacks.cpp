#include "callbacks.hpp"
#include "GLFW/glfw3.h"
#include "camera.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <iostream>

using renderer::camera::CAMERA_STATE;

using namespace window;

void callback::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    //glViewport(0, 0, width, height);
}

void callback::process_input(GLFWwindow *window, float delta_frame)
{

    using renderer::camera::Camera_Movement;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        CAMERA_STATE.ProcessMovement(Camera_Movement::FORWARD, delta_frame);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        CAMERA_STATE.ProcessMovement(Camera_Movement::BACKWARD, delta_frame);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        CAMERA_STATE.ProcessMovement(Camera_Movement::LEFT, delta_frame);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        CAMERA_STATE.ProcessMovement(Camera_Movement::RIGHT, delta_frame);
}

void callback::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    static bool first_mouse = true;
    static unsigned int last_x = 0;
    static unsigned int last_y = 0;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (xpos < 0.0 || ypos < 0.0)
        return;

    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

    last_x = xpos;
    last_y = ypos;

    // process only drag
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE) {
        CAMERA_STATE.ProcessMouseMovement(xoffset, yoffset);
    }
}

void callback::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    CAMERA_STATE.ProcessMouseScroll(static_cast<float>(yoffset));
}
