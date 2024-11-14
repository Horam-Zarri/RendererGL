#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Light.hpp"

class PointLight : public Light {
    GENERATE_PTR(PointLight)
protected:
    glm::vec3 m_Position;
    Attenuation m_Attenuation;

    Attenuation dist_to_atten(unsigned int distance) {
        Attenuation near, far;
        float delta;

        auto it = attenuation_table.cbegin();
        auto end = attenuation_table.cend();

        while (++it != end) {
            unsigned int ed = it->first;
            if (ed >= distance) {
                far = it->second;

                unsigned int bd = (--it)->first;
                near = it++->second;

                delta = ((float)(distance - bd)) / (ed - bd);
                std::cout << "DISTANCE " << distance << " BD " << ed << " ED" << bd << std::endl;
                std::cout << "DELTA " << delta << std::endl;
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
    ) : Light(ambient, diffuse, specular),
        m_Position{position}, m_Attenuation{attenuation}
    {
        m_Type = LightType::PointLight;
    }

    PointLight(
        glm::vec3 position,
        unsigned int distance,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular
    ) : Light(ambient, diffuse, specular), m_Position{position}
    {
        m_Type = LightType::PointLight;

        // TODO: do something with magic numbers
        if (distance < 7 || distance > 3250) {
            std::cerr << "Point light distance should be >=7, <=3250";
            std::exit(7);
        }

        m_Attenuation = dist_to_atten(distance);
    }

    PointLight(
        glm::vec3 position,
        Attenuation attenuation,
        glm::vec3 light_color
    ) : Light(light_color), m_Position{position}, m_Attenuation{attenuation}
    {
        m_Type = LightType::PointLight;
    }

    PointLight(
        glm::vec3 position,
        unsigned int distance,
        glm::vec3 light_color
    ) : Light(light_color), m_Position{position}
    {
        m_Type = LightType::PointLight;
        if (distance < 7 || distance > 3250) {
            std::cerr << "Point light distance should be >=7, <=3250";
            std::exit(7);
        }

        m_Attenuation = dist_to_atten(distance);

        std::cout << "CALCULATED_ATTENUATION" << m_Attenuation.linear << " " <<
            m_Attenuation.quadratic << std::endl;
    }

    virtual ~PointLight() = default;

    inline const glm::vec3& getPosition() const {
        return m_Position;
    }
    inline const Attenuation& getAttenuation() const {
        return m_Attenuation;
    }
    inline void setPosition(const glm::vec3& position) {
        this->m_Position = position;
    }

    inline void setAttenuation(const Attenuation attenuation) {
        this->m_Attenuation = attenuation;
    }

    inline void setDistance(const unsigned int distance) {
        this->m_Attenuation = dist_to_atten(distance);
    }
};
#endif
