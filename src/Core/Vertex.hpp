#ifndef VERTEX_H
#define VERTEX_H

#include "Core/VertexArray.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
};

static inline VBLayout defaultVertexLayout() {
    VBLayout layout;

    layout.push<float>(3); // Position
    layout.push<float>(3); // Normal
    layout.push<float>(2); // TexCoord
    layout.push<float>(3); // Tangent
    layout.push<float>(3); // Bitangent

    return layout;
}

static inline glm::vec3 calculateNormal(
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    bool normalized = true
) {

    const glm::vec3 Edge1 = P2 - P1;
    const glm::vec3 Edge2 = P3 - P1;

    glm::vec3 N = glm::cross(Edge2, Edge1);

    return normalized ?
        glm::normalize(N) :
        N;
}

static inline std::pair<glm::vec3, glm::vec3> calculateTangentBitangent(
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const glm::vec2& TX1,
    const glm::vec2& TX2,
    const glm::vec2& TX3
) {

    const glm::vec3 Edge1 = P2 - P1;
    const glm::vec3 Edge2 = P3 - P1;

    const glm::vec2 DeltaUV1 = TX2 - TX1;
    const glm::vec2 DeltaUV2 = TX3 - TX1;

    glm::vec3 Tangent, Bitangent;

    float F = 1.0f / (DeltaUV1.x * DeltaUV2.y - DeltaUV2.x * DeltaUV1.y);

    Tangent.x = F * (DeltaUV2.y * Edge1.x - DeltaUV1.y * Edge2.x);
    Tangent.y = F * (DeltaUV2.y * Edge1.y - DeltaUV1.y * Edge2.y);
    Tangent.z = F * (DeltaUV2.y * Edge1.z - DeltaUV1.y * Edge2.z);

    Bitangent.x = F * (-DeltaUV2.x * Edge1.x + DeltaUV1.x * Edge2.x);
    Bitangent.y = F * (-DeltaUV2.x * Edge1.y + DeltaUV1.x * Edge2.y);
    Bitangent.z = F * (-DeltaUV2.x * Edge1.z + DeltaUV1.x * Edge2.z);

    return std::make_pair(Tangent, Bitangent);
}

static inline std::vector<Vertex>& generateNormals(std::vector<Vertex>& vertices)
{
    for (unsigned int i = 0; i < vertices.size(); i += 3) {
        vertices[i].Normal = vertices[i + 1].Normal = vertices[i + 2].Normal =
        calculateNormal(
            vertices[i].Position,
            vertices[i + 1].Position,
            vertices[i + 2].Position
        );
    }
    return vertices;
}

#endif
