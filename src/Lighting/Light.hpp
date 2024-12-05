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

    inline const LightType getType() const { return m_Type; }

    inline const glm::vec3& getAmbient() const { return m_Ambient; }
    inline const glm::vec3& getDiffuse() const { return m_Diffuse; }
    inline const glm::vec3& getSpecular() const { return m_Specular; }

    inline const glm::vec3 getAveragedColor() const {
        return (m_Ambient + m_Diffuse + m_Specular) / 3.f;
    }

    inline const glm::vec3 getAveragedColorClamp(float clamp_v = 1.f) const {
        glm::vec3 avg = getAveragedColor();
        float max = std::fmax(std::fmax(avg.r, avg.b), avg.g);
        avg *= (clamp_v / max);
        return avg;
    }

    inline void setAmbient(const glm::vec3& ambient) { m_Ambient = ambient; }
    inline void setDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse; }
    inline void setSpecular(const glm::vec3& specular) { m_Specular = specular; }

    inline void setColor(const glm::vec3& light_color) {
        m_Ambient = m_Diffuse = m_Specular = light_color;
    }
};




#endif
