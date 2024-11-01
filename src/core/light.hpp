#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

const std::vector<std::pair<unsigned int, Attenuation>> attenuation_table = {
    {7 , Attenuation {1.0, 0.7, 1.8}},
    {13, Attenuation {1.0, 0.35, 0.44}},
    {20, Attenuation {1.0, 0.22,  0.20}},
    {32, Attenuation {1.0, 0.14,  0.07}},
    {50, Attenuation {1.0, 0.09,  0.032}},
    {65, Attenuation {1.0, 0.07,  0.017}},
    {100, Attenuation {1.0, 0.045,  0.0075}},
    {160, Attenuation {1.0, 0.027,  0.0028}},
    {200, Attenuation {1.0, 0.022,  0.0019}},
    {325, Attenuation {1.0, 0.014,  0.0007}},
    {600, Attenuation {1.0, 0.007,  0.0002}},
    {3250, Attenuation {1.0, 0.0014,  0.000007}},
};

class PointLight {
private:
    glm::vec3 position;

    Attenuation attenuation;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Attenuation dist_to_atten(unsigned int distance) {
        Attenuation near, far;
        float scale_between;

        auto it = attenuation_table.cbegin();
        auto end = attenuation_table.cend();

        while (it != end) {
            unsigned int bd = it->first;
            if (bd >= distance) {
                near = it->second;
                it++;
                unsigned int ed = it->first;
                if (it != end)
                    far = it->second;
                else
                    far = near;

                scale_between = ((float)(distance - bd)) / (ed - bd);
                break;
            }
        }

        // linear scale of all parameters is not accurate
        // but suffices for small distances
        return Attenuation {
            .constant = near.constant +
            (far.constant - near.constant) * scale_between,
            .linear = near.linear +
            (far.linear - near.linear) * scale_between,
            .quadratic = near.quadratic +
            (far.quadratic - near.quadratic) * scale_between,
        };
    }

public:

    PointLight(
        glm::vec3 position,
        Attenuation attenuation,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular
    ) : position{position}, attenuation{attenuation},
        ambient{ambient}, diffuse{diffuse}, specular{specular} {}

    PointLight(
        glm::vec3 position,
        unsigned int distance,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular
    ) : position{position}, ambient{ambient},
        diffuse{diffuse}, specular{specular}
    {
        // TODO: do something with magic numbers
        if (distance < 7 || distance > 3250) {
            std::cerr << "Point light distance should be >=7, <=3250";
            std::exit(7);
        }

        attenuation = dist_to_atten(distance);
    }

    PointLight(
        glm::vec3 position,
        Attenuation attenuation,
        glm::vec3 light_color
    ) : position{position}, attenuation{attenuation}, ambient{light_color},
        diffuse{light_color}, specular{light_color}  {}

    PointLight(
        glm::vec3 position,
        unsigned int distance,
        glm::vec3 light_color
    ) : position{position}, ambient{light_color},
        diffuse{light_color}, specular{light_color}
    {
        if (distance < 7 || distance > 3250) {
            std::cerr << "Point light distance should be >=7, <=3250";
            std::exit(7);
        }

        attenuation = dist_to_atten(distance);
    }
};

#endif
