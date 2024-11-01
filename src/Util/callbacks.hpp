#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace window::callback
{
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window, float delta_frame);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
}

#endif
