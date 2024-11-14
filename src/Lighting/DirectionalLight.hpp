#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "Light.hpp"

class DirectionalLight : public Light {
    GENERATE_PTR(DirectionalLight)
private:
    glm::vec3 m_Direction;
public:

    DirectionalLight(
        glm::vec3 direction,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular
    ) : Light(ambient, diffuse, specular), m_Direction{direction}
    {
        m_Type = LightType::Directional;
    }


    DirectionalLight(
        glm::vec3 direction,
        glm::vec3 light_color
    ) : Light(light_color), m_Direction{direction}
    {
        m_Type = LightType::Directional;
    }

    virtual ~DirectionalLight() = default;

    inline void setDirection(const glm::vec3& direction) {
        this->m_Direction = direction;
    }

    inline const glm::vec3& getDirection() const {
        return m_Direction;
    }
};

#endif

