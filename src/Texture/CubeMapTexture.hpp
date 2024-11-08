#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"
#include "Texture.hpp"


class CubeMapTexture : public Texture {
    MAKE_MOVE_ONLY(CubeMapTexture)
    GENERATE_PTR(CubeMapTexture)

private:
    void genTexture() override;
public:
    std::array<std::string, 6> m_Paths;

    CubeMapTexture(
        const std::array<std::string, 6>& faces,
        TextureConfig tex_conf = TextureConfig()
    );

    void bind() const override;
    void unbind() const override;
};

#endif
