#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "window.hpp"
#include "renderer.hpp"

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

        renderer::update_state();
        renderer::render();

        window::render_gui();

        window::swap_and_poll();
    }

    renderer::terminate();
    window::terminate();
    return 0;
}

//unsigned int loadTexture(char const *path)
//{
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    int width, height, nrComponents;
//    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
//    if (data)
//    {
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;
//
//        glBindTexture(GL_TEXTURE_2D, textureID);
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//{
//        std::cout << "Texture failed to load at path: " << path << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
//}
