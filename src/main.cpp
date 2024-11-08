#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Renderer/Window.hpp"
#include "Renderer/Renderer.hpp"

int main()
{
    if (window::init() != 0) {
        std::cerr << "Could not initialize window!" << std::endl;
        window::terminate();
    }
    if (renderer::init() != 0) {
        std::cerr << "Could not initialize renderer!";
        renderer::terminate();
        window::terminate();
    }
    std::cout << "AFTER_RENDERER_INIT" << std::endl;
    while(!window::should_terminate())
    {
        window::handle_input();

        renderer::updateState();
        renderer::render();

        window::render_gui();

        window::swap_and_poll();
    }

    renderer::terminate();
    window::terminate();
    return 0;
}
