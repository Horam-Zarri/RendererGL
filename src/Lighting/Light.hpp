#ifndef LIGHT_H
#define LIGHT_H

#include "Core/Shader/Shader.hpp"
#include "Util/Ptr.hpp"

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

enum class LightType {
    None = 0,
    Directional,
    PointLight,
    SpotLight,
};

class Light {
    GENERATE_PTR(Light)
private:
    glm::vec3 m_Ambient;
    glm::vec3 m_Diffuse;
    glm::vec3 m_Specular;

protected:
    LightType m_Type;

    Light(
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular
    ) : m_Ambient{ambient}, m_Diffuse{diffuse}, m_Specular{specular} {}

    Light(
        glm::vec3 light_color
    ) : m_Ambient{light_color}, m_Diffuse{light_color}, m_Specular{light_color} {}

    virtual ~Light() = default;

public:

    inline const LightType getType() const {
        return m_Type;
    }
    inline const glm::vec3& getAmbient() const {
        return m_Ambient;
    }
    inline const glm::vec3& getDiffuse() const {
        return m_Diffuse;
    }
    inline const glm::vec3& getSpecular() const {
        return m_Specular;
    }

    inline void setAmbient(const glm::vec3& ambient) {
        this->m_Ambient = ambient;
    }
    inline void setDiffuse(const glm::vec3& diffuse) {
        this->m_Diffuse = diffuse;
    }
    inline void setSpecular(const glm::vec3& specular) {
        this->m_Specular = specular;
    }
};

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

class SpotLight : public PointLight {
    GENERATE_PTR(SpotLight)
private:
    glm::vec3 m_Direction;
    float m_CutOff;
    float m_OuterCutOff;

    inline float toCosineRadian(float deg) const { return glm::cos(glm::radians(deg));}
    inline float toDegrees(float radcos) const { return glm::degrees(glm::acos(radcos));}
public:

    SpotLight(
        glm::vec3 position,
        glm::vec3 direction,
        Attenuation attenuation,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float cut_off_deg,
        float outer_cut_off_deg
    ) :
        PointLight(position, attenuation, ambient, diffuse, specular),
        m_Direction{direction},
        m_CutOff{glm::cos(glm::radians(cut_off_deg))},
        m_OuterCutOff{glm::cos(glm::radians(outer_cut_off_deg))}
    {
        m_Type = LightType::SpotLight;
    }


    SpotLight(
        glm::vec3 position,
        glm::vec3 direction,
        unsigned int distance,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float cut_off_deg,
        float outer_cut_off_deg
    ) :
        PointLight(position, distance, ambient, diffuse, specular),
        m_Direction{direction},
        m_CutOff{glm::cos(glm::radians(cut_off_deg))},
        m_OuterCutOff{glm::cos(glm::radians(outer_cut_off_deg))}
    {
        m_Type = LightType::SpotLight;
    }


    SpotLight(
        glm::vec3 position,
        glm::vec3 direction,
        Attenuation attenuation,
        glm::vec3 light_color,
        float cut_off_deg,
        float outer_cut_off_deg
    ) :
        PointLight(position, attenuation, light_color),
        m_Direction{direction},
        m_CutOff{glm::cos(glm::radians(cut_off_deg))},
        m_OuterCutOff{glm::cos(glm::radians(outer_cut_off_deg))}
    {
        m_Type = LightType::SpotLight;
    }

    SpotLight(
        glm::vec3 position,
        glm::vec3 direction,
        unsigned int distance,
        glm::vec3 light_color,
        float cut_off_deg,
        float outer_cut_off_deg
    ) :
        PointLight(position, distance, light_color),
        m_Direction{direction},
        m_CutOff{glm::cos(glm::radians(cut_off_deg))},
        m_OuterCutOff{glm::cos(glm::radians(outer_cut_off_deg))}
    {
        m_Type = LightType::SpotLight;
    }

    inline void setDirection(const glm::vec3& direction) { m_Direction = direction; }
    inline const glm::vec3& getDirection() const { return m_Direction; }

    inline void setCutOff(float deg) { m_CutOff = toCosineRadian(deg); }
    inline float getCutOff() const { return m_CutOff; }
    inline float getCutOffDeg() const { return toDegrees(m_CutOff); }

    inline void setOuterCutOff(float deg) { m_OuterCutOff = toCosineRadian(deg); }
    inline float getOuterCutOff() const { return m_OuterCutOff; }
    inline float getOuterCutOffDeg() const { return toDegrees(m_OuterCutOff); }

    virtual ~SpotLight() = default;
};

#endif
