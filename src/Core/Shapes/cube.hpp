#ifndef CUBE_H
#define CUBE_H

#include "Core/Mesh.hpp"
#include "Core/Vertex.hpp"
#include "Core/VertexBuffer.hpp"
#include "Core/VertexArray.hpp"
#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

static std::vector<Vertex> cubeVertices = {
    Vertex {.Position = glm::vec3(-0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(-0.5f,  0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(-0.5f, -0.5f, -0.5f)},

    Vertex {.Position = glm::vec3(-0.5f, -0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f, -0.5f,  0.5f)},

    Vertex {.Position = glm::vec3(-0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f,  0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(-0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(-0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(-0.5f, -0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f,  0.5f,  0.5f)},

    Vertex {.Position = glm::vec3(0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f,  0.5f)},

    Vertex {.Position = glm::vec3(-0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(0.5f, -0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f, -0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f, -0.5f, -0.5f)},

    Vertex {.Position = glm::vec3(-0.5f,  0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f, -0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f,  0.5f,  0.5f)},
    Vertex {.Position = glm::vec3(-0.5f,  0.5f, -0.5f)},
};

class Cube : public Mesh {
    MAKE_MOVE_ONLY(Cube)
    GENERATE_PTR(Cube)
public:

    Cube() : Mesh(cubeVertices) {}

};

#endif
