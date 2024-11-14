#ifndef PHONG_MATERIAL_H
#define PHONG_MATERIAL_H

#include "Material.hpp"

class PhongMaterial: public Material {
    GENERATE_PTR(PhongMaterial)
private:
    glm::vec3 m_Ambient;
    glm::vec3 m_Diffuse;
    glm::vec3 m_Specular;

    float m_Shininess;
public:

    PhongMaterial(float shininess = 32):
        Material(MaterialType::Phong),
        m_Ambient{1.0f}, m_Diffuse{1.0f}, m_Specular{1.0f},
        m_Shininess{shininess} {}

    PhongMaterial(
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        float shininess = 32
    ) : Material(MaterialType::Phong),
        m_Ambient{ambient}, m_Diffuse{diffuse}, m_Specular{specular},
        m_Shininess{shininess} {}


    inline void setAmbient(const glm::vec3& ambient) { m_Ambient = ambient; }
    inline const glm::vec3& getAmbient() const { return m_Ambient; }

    inline void setDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse; }
    inline const glm::vec3& getDiffuse() const { return m_Diffuse; }

    inline void setSpecular(const glm::vec3& specular) { m_Specular = specular; }
    inline const glm::vec3& getSpecular() const { return m_Specular; }

    inline void setShininess(const float shininess) { m_Shininess = shininess; }
    inline float getShininess() const { return m_Shininess; }
};

#endif
