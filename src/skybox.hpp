#ifndef SKYBOX_H
#define SKYBOX_H

#include "camera.hpp"
#include "core/VertexArray.hpp"
#include "core/VertexBuffer.hpp"
#include "core/cubemap.hpp"
#include "shader.hpp"


class Skybox {
private:
    Cubemap m_Skymap;

    VertexBuffer m_VBO;
    VertexArray m_VAO;
public:

    Skybox(std::array<std::string, 6> faces);
    void Draw(Shader& shader, const renderer::camera::Camera& camera);

};

#endif
