#ifndef SKYBOX_H
#define SKYBOX_H

#include "Camera.hpp"
#include "Core/VertexArray.hpp"
#include "Core/VertexBuffer.hpp"
#include "Texture/CubeMapTexture.hpp"
#include "Core/Shader/Shader.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"


class Skybox {
    MAKE_MOVE_ONLY(Skybox)
    GENERATE_PTR(Skybox)
private:
    CubeMapTexture::Ptr m_Skymap;

    VertexBuffer::Ptr m_VBO;
    VertexArray::Ptr m_VAO;

public:
    Skybox(std::array<std::string, 6> faces);

    void draw();
};

#endif
