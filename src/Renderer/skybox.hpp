#ifndef SKYBOX_H
#define SKYBOX_H

#include "camera.hpp"
#include "Core/VertexArray.hpp"
#include "Core/VertexBuffer.hpp"
#include "Texture/cubemap.hpp"
#include "Core/Shader/shader.hpp"


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
