#ifndef SKYBOX_H
#define SKYBOX_H

#include "Camera.hpp"
#include "Core/VertexArray.hpp"
#include "Core/VertexBuffer.hpp"
#include "Texture/CubeMapBufferTexture.hpp"
#include "Texture/CubeMapTexture.hpp"
#include "Core/Shader/Shader.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"


class Skybox {
    MAKE_MOVE_ONLY(Skybox)
    GENERATE_PTR(Skybox)
private:
    static inline TextureConfig defaultConfig() {
        TextureConfig tconf;
        tconf.wrap_r = tconf.wrap_s = tconf.wrap_t = GL_CLAMP_TO_EDGE;
        tconf.min_filter = tconf.mag_filter = GL_LINEAR;
        tconf.flip = false;
        return tconf;
    }

    CubeMapTexture::Ptr m_SkymapTexture;
    CubeMapBufferTexture::Ptr m_SkymapBuffer;

    VertexBuffer::Ptr m_VBO;
    VertexArray::Ptr m_VAO;

public:
    Skybox(std::array<std::string, 6> faces);
    Skybox(const CubeMapBufferTexture::Ptr& cubemap_buffer);
    void draw();
};

#endif
