#ifndef WINDOW_H
#define WINDOW_H

#include "gui.hpp"
namespace window
{

constexpr static unsigned int WINDOW_WIDTH = 1280;
constexpr static unsigned int WINDOW_HEIGHT = 720;

static GLFWwindow* _Window;

static double delta_frame;
static double last_frame = 0;

int init();
void resize_window(int width, int height);
bool should_terminate();
void poll_glfw();
void handle_input();
void render_gui();
void swap_and_poll();
void terminate();

}

#endif
