#ifndef PLANE_H
#define PLANE_H

#include "Core/Mesh.hpp"

#include "Util/MoveOnly.hpp"
#include "Util/Ptr.hpp"

static std::vector<Vertex> planeVertices = {
    // positions            // normals         // texcoords
    Vertex {.Position = glm::vec3(25.0f, -0.5f,  25.0f), .Normal =  glm::vec3(0.0f, 1.0f, 0.0f), .TexCoords =  glm::vec2(25.0f,  0.0f) },
    Vertex {.Position = glm::vec3(-25.0f, -0.5f,  25.0f), .Normal =  glm::vec3(0.0f, 1.0f, 0.0f), .TexCoords = glm::vec2(   0.0f,  0.0f)},
    Vertex {.Position = glm::vec3(-25.0f, -0.5f, -25.0f), .Normal =  glm::vec3(0.0f, 1.0f, 0.0f), .TexCoords = glm::vec2(   0.0f, 25.0f)},

    Vertex {.Position = glm::vec3(25.0f, -0.5f,  25.0f), .Normal =  glm::vec3(0.0f, 1.0f, 0.0f), .TexCoords = glm::vec2(  25.0f,  0.0f)},
    Vertex {.Position = glm::vec3(-25.0f, -0.5f, -25.0f), .Normal =  glm::vec3(0.0f, 1.0f, 0.0f), .TexCoords = glm::vec2(   0.0f, 25.0f)},
    Vertex {.Position = glm::vec3(25.0f, -0.5f, -25.0f), .Normal =  glm::vec3(0.0f, 1.0f, 0.0f), .TexCoords = glm::vec2(  25.0f, 25.0f)},
};

class Plane : public Mesh {
    MAKE_MOVE_ONLY(Plane)
    GENERATE_PTR(Plane)

public:
    Plane() : Mesh(planeVertices) {}
};

#endif