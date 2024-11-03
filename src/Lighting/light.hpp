#ifndef LIGHT_H
#define LIGHT_H

#include "Core/Shader/shader.hpp"
#include <format>
#include <glm/vec3.hpp>
#include <stdexcept>
#include <string>
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

class DirectionalLight {
private:
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

public:

    DirectionalLight(
        glm::vec3 direction,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular
    ) : direction{direction}, ambient{ambient},
        diffuse{diffuse}, specular{specular} {}

    DirectionalLight(
        glm::vec3 direction,
        glm::vec3 light_color
    ) : direction{direction}, ambient{light_color},
        diffuse{light_color}, specular{light_color} {}

    inline void setDirection(const glm::vec3& direction) {
        this->direction = direction;
    }
    inline void setAmbient(const glm::vec3& ambient) {
        this->ambient = ambient;
    }
    inline void setDiffuse(const glm::vec3& diffuse) {
        this->diffuse = diffuse;
    }
    inline void setSpecular(const glm::vec3& specular) {
        this->specular = specular;
    }

    void sendUniforms(Shader& shader) const {
        shader.setVec3("directionalLight.direction", direction);
        shader.setVec3("directionalLight.ambient", ambient);
        shader.setVec3("directionalLight.diffuse", diffuse);
        shader.setVec3("directionalLight.specular", specular);
    }

};

class PointLight {
protected:
    glm::vec3 position;

    Attenuation attenuation;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Attenuation dist_to_atten(unsigned int distance) {
        Attenuation near, far;
        float delta;

        auto it = attenuation_table.cbegin();
        auto end = attenuation_table.cend();

        for (; it != end; ++it) {
            unsigned int bd = it->first;
            if (bd >= distance) {
                near = it->second;
                it++;
                unsigned int ed = it->first;
                if (it != end)
                    far = it->second;
                else
                    far = near;

                delta = ((float)(distance - bd)) / (ed - bd);
                std::cout << "DISTANCE " << distance << " BD " << bd << " ED" << ed << std::endl;
                break;
            }
        }

        // linear scale of all parameters is not accurate
        // but suffices for small distances
        return Attenuation {
            .constant = near.constant +
            (far.constant - near.constant) * delta,
            .linear = near.linear +
            (far.linear - near.linear) * delta,
            .quadratic = near.quadratic +
            (far.quadratic - near.quadratic) * delta,
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

        std::cout << "CALCULATED_ATTENUATION" << attenuation.linear << " " <<
            attenuation.quadratic << std::endl;
    }

    inline const glm::vec3& getPosition() const {
        return position;
    }

    inline const glm::vec3& getAmbient() const {
        return ambient;
    }

    inline const glm::vec3& getDiffuse() const {
        return diffuse;
    }

    inline const glm::vec3& getSpecular() const {
        return specular;
    }

    inline const Attenuation& getAttenuation() const {
        return attenuation;
    }
    inline void setPosition(const glm::vec3& position) {
        this->position = position;
    }

    inline void setAttenuation(const Attenuation attenuation) {
        this->attenuation = attenuation;
    }

    inline void setDistance(const unsigned int distance) {
        this->attenuation = dist_to_atten(distance);
    }

    inline void setAmbient(const glm::vec3& ambient) {
        this->ambient = ambient;
    }

    inline void setDiffuse(const glm::vec3& diffuse) {
        this->diffuse = diffuse;
    }

    inline void setSpecular(const glm::vec3& specular) {
        this->specular = specular;
    }

    void sendUniforms(Shader& shader, int slot) const {
        std::string ubase = "pointLights[" + std::to_string(slot) + "]";

        shader.setVec3(ubase + ".position", position);
        shader.setVec3(ubase + ".ambient", ambient);
        shader.setVec3(ubase + ".diffuse", diffuse);
        shader.setVec3(ubase + ".specular", specular);

        shader.setFloat(ubase + ".constant", attenuation.constant);
        shader.setFloat(ubase + ".linear", attenuation.linear);
        shader.setFloat(ubase + ".quadratic", attenuation.quadratic);
    }
};

class SpotLight : public PointLight {
private:
    glm::vec3 direction;
    float cut_off;
    float outer_cut_off;

public:

    SpotLight(
        glm::vec3 position,
        glm::vec3 direction,
        Attenuation attenuation,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float cut_off,
        float outer_cut_off
    ) : PointLight(position, attenuation, ambient, diffuse, specular),
        direction{direction}, cut_off{cut_off}, outer_cut_off{outer_cut_off} {}


    SpotLight(
        glm::vec3 position,
        glm::vec3 direction,
        unsigned int distance,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float cut_off,
        float outer_cut_off
    ) : PointLight(position, distance, ambient, diffuse, specular),
        direction{direction}, cut_off{cut_off}, outer_cut_off{outer_cut_off} {}


    SpotLight(
        glm::vec3 position,
        glm::vec3 direction,
        Attenuation attenuation,
        glm::vec3 light_color,
        float cut_off,
        float outer_cut_off
    ) : PointLight(position, attenuation, light_color),
        direction{direction}, cut_off{cut_off}, outer_cut_off{outer_cut_off} {}

};

#endif
