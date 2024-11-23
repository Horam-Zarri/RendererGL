#ifndef SPHERE_H
#define SPHERE_H

#include <glm/vec3.hpp>
#include "Core/Mesh.hpp"
#include "Texture/Texture.hpp"
#include "Util/Ptr.hpp"
#include "Util/MoveOnly.hpp"
#include <functional>

constexpr static double PI = 3.141592653589793238463;

constexpr static std::vector<Vertex> genSphereVertices(
    const unsigned int X_Segments,
    const unsigned int Y_Segments
) {
    std::vector<Vertex> vertices;
    for (unsigned int x = 0; x <= X_Segments; ++x)
    {
        for (unsigned int y = 0; y <= Y_Segments; ++y)
        {
            float xSegment = (float)x / (float)X_Segments;
            float ySegment = (float)y / (float)Y_Segments;

            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            Vertex v;
            v.Position = glm::vec3(xPos, yPos, zPos);
            v.TexCoords = glm::vec2(xSegment, ySegment);
            v.Normal = glm::vec3(xPos, yPos, zPos);

            vertices.push_back(v);
        }
    }
    return vertices;
}

constexpr static std::vector<unsigned int> genSphereIndices(
    const unsigned int X_Segments,
    const unsigned int Y_Segments
) {
    std::vector<unsigned int> indices;
    bool oddRow = false;
    for (unsigned int y = 0; y < Y_Segments; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_Segments; ++x)
            {
                indices.push_back(y       * (X_Segments + 1) + x);
                indices.push_back((y + 1) * (X_Segments + 1) + x);
            }
        }
        else
        {
            // unsigned int will crash cause x hits -1
            for (int x = X_Segments; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_Segments + 1) + x);
                indices.push_back(y       * (X_Segments + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    return indices;
}

class Sphere : public Mesh {
    MAKE_MOVE_ONLY(Sphere)
    GENERATE_PTR(Sphere)
public:
    Sphere(
        const unsigned int X_Segments,
        const unsigned int Y_Segments
    ) :
    Mesh(
        std::move(genSphereVertices(X_Segments, Y_Segments)),
        std::move(genSphereIndices(X_Segments, Y_Segments))
    ) {

    }

    virtual void draw(bool wireframe, GLenum primitive) override
    {
        Mesh::draw(wireframe, GL_TRIANGLE_STRIP);
    }
};
#endif
