#ifndef CUBE_H
#define CUBE_H

#include "Core/Mesh.hpp"
#include "Core/Vertex.hpp"
#include "Core/VertexBuffer.hpp"
#include "Core/VertexArray.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

static std::vector<Vertex>  cubeVertices = {
    // back face
    Vertex {.Position = vec3(-1.0f, -1.0f, -1.0f),  .Normal = vec3(0.0f,  0.0f, -1.0f), .TexCoords = vec2(0.0f, 0.0f)}, // bottom-left
    Vertex {.Position = vec3(1.0f,  1.0f, -1.0f),  .Normal = vec3(0.0f,  0.0f, -1.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-right
    Vertex {.Position = vec3(1.0f, -1.0f, -1.0f),  .Normal = vec3(0.0f,  0.0f, -1.0f), .TexCoords = vec2(1.0f, 0.0f)}, // bottom-right
    Vertex {.Position = vec3(1.0f,  1.0f, -1.0f),  .Normal = vec3(0.0f,  0.0f, -1.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-right
    Vertex {.Position = vec3(-1.0f, -1.0f, -1.0f),  .Normal = vec3(0.0f,  0.0f, -1.0f), .TexCoords = vec2(0.0f, 0.0f)}, // bottom-left
    Vertex {.Position = vec3(-1.0f,  1.0f, -1.0f),  .Normal = vec3(0.0f,  0.0f, -1.0f), .TexCoords = vec2(0.0f, 1.0f)}, // top-left
    // front face
    Vertex {.Position = vec3(-1.0f, -1.0f,  1.0f),  .Normal = vec3(0.0f,  0.0f,  1.0f), .TexCoords = vec2(0.0f, 0.0f)}, // bottom-left
    Vertex {.Position = vec3(1.0f, -1.0f,  1.0f),  .Normal = vec3(0.0f,  0.0f,  1.0f), .TexCoords = vec2(1.0f, 0.0f)}, // bottom-right
    Vertex {.Position = vec3(1.0f,  1.0f,  1.0f),  .Normal = vec3(0.0f,  0.0f,  1.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-right
    Vertex {.Position = vec3(1.0f,  1.0f,  1.0f),  .Normal = vec3(0.0f,  0.0f,  1.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-right
    Vertex {.Position = vec3(-1.0f,  1.0f,  1.0f),  .Normal = vec3(0.0f,  0.0f,  1.0f), .TexCoords = vec2(0.0f, 1.0f)}, // top-left
    Vertex {.Position = vec3(-1.0f, -1.0f,  1.0f),  .Normal = vec3(0.0f,  0.0f,  1.0f), .TexCoords = vec2(0.0f, 0.0f)}, // bottom-left
    // left face
    Vertex {.Position = vec3(-1.0f,  1.0f,  1.0f), .Normal = vec3(-1.0f,  0.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // top-right
    Vertex {.Position = vec3(-1.0f,  1.0f, -1.0f), .Normal = vec3(-1.0f,  0.0f,  0.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-left
    Vertex {.Position = vec3(-1.0f, -1.0f, -1.0f), .Normal = vec3(-1.0f,  0.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // bottom-left
    Vertex {.Position = vec3(-1.0f, -1.0f, -1.0f), .Normal = vec3(-1.0f,  0.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // bottom-left
    Vertex {.Position = vec3(-1.0f, -1.0f,  1.0f), .Normal = vec3(-1.0f,  0.0f,  0.0f), .TexCoords = vec2(0.0f, 0.0f)}, // bottom-right
    Vertex {.Position = vec3(-1.0f,  1.0f,  1.0f), .Normal = vec3(-1.0f,  0.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // top-right
    // right face
    Vertex {.Position = vec3(1.0f,  1.0f,  1.0f),  .Normal = vec3(1.0f,  0.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // top-left
    Vertex {.Position = vec3(1.0f, -1.0f, -1.0f),  .Normal = vec3(1.0f,  0.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // bottom-right
    Vertex {.Position = vec3(1.0f,  1.0f, -1.0f),  .Normal = vec3(1.0f,  0.0f,  0.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-right
    Vertex {.Position = vec3(1.0f, -1.0f, -1.0f),  .Normal = vec3(1.0f,  0.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // bottom-right
    Vertex {.Position = vec3(1.0f,  1.0f,  1.0f),  .Normal = vec3(1.0f,  0.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // top-left
    Vertex {.Position = vec3(1.0f, -1.0f,  1.0f),  .Normal = vec3(1.0f,  0.0f,  0.0f), .TexCoords = vec2(0.0f, 0.0f)}, // bottom-left
    // bottom face
    Vertex {.Position = vec3(-1.0f, -1.0f, -1.0f),  .Normal = vec3(0.0f, -1.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // top-right
    Vertex {.Position = vec3(1.0f, -1.0f, -1.0f),  .Normal = vec3(0.0f, -1.0f,  0.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-left
    Vertex {.Position = vec3(1.0f, -1.0f,  1.0f),  .Normal = vec3(0.0f, -1.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // bottom-left
    Vertex {.Position = vec3(1.0f, -1.0f,  1.0f),  .Normal = vec3(0.0f, -1.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // bottom-left
    Vertex {.Position = vec3(-1.0f, -1.0f,  1.0f),  .Normal = vec3(0.0f, -1.0f,  0.0f), .TexCoords = vec2(0.0f, 0.0f)}, // bottom-right
    Vertex {.Position = vec3(-1.0f, -1.0f, -1.0f),  .Normal = vec3(0.0f, -1.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // top-right
    // top face
    Vertex {.Position = vec3(-1.0f,  1.0f, -1.0f),  .Normal = vec3(0.0f,  1.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // top-left
    Vertex {.Position = vec3(1.0f,  1.0f , 1.0f),  .Normal = vec3(0.0f,  1.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // bottom-right
    Vertex {.Position = vec3(1.0f,  1.0f, -1.0f),  .Normal = vec3(0.0f,  1.0f,  0.0f), .TexCoords = vec2(1.0f, 1.0f)}, // top-right
    Vertex {.Position = vec3(1.0f,  1.0f,  1.0f),  .Normal = vec3(0.0f,  1.0f,  0.0f), .TexCoords = vec2(1.0f, 0.0f)}, // bottom-right
    Vertex {.Position = vec3(-1.0f,  1.0f, -1.0f),  .Normal = vec3(0.0f,  1.0f,  0.0f), .TexCoords = vec2(0.0f, 1.0f)}, // top-left
    Vertex {.Position = vec3(-1.0f,  1.0f,  1.0f),  .Normal = vec3(0.0f,  1.0f,  0.0f), .TexCoords = vec2(0.0f, 0.0f)}  // bottom-left
};

class Cube : public Mesh {
    MAKE_MOVE_ONLY(Cube)
    GENERATE_PTR(Cube)
    public:

    Cube() : Mesh(cubeVertices) {}

};

#endif
