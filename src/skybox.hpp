#ifndef SKYBOX_H
#define SKYBOX_H

#include "camera.hpp"
#include "core/VAO.hpp"
#include "core/VBO.hpp"
#include "core/cubemap.hpp"
#include "shader.hpp"


class Skybox {
private:
    Cubemap m_Skymap;

    VBO m_VBO;
    VAO m_VAO;
public:

    Skybox(std::array<std::string, 6> faces);
    void Draw(Shader& shader, const renderer::camera::Camera& camera);

};

#endif
