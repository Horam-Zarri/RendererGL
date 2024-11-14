#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include "PointLight.hpp"


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
