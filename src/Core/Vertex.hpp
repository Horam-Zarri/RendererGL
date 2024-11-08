#ifndef VERTEX_H
#define VERTEX_H

#include "Core/VertexArray.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace glm;

struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
};

static inline VBLayout defaultVertexLayout() {
    VBLayout layout;

    layout.push<float>(3); // Position
    layout.push<float>(3); // Normal
    layout.push<float>(2); // TexCoord

    return layout;
}

#endif
