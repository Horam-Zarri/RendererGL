#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>
#include "Texture/Texture.hpp"

#include "Core/Shader/Shader.hpp"
#include "Util/Ptr.hpp"
#include <variant>

class Material {
    GENERATE_PTR(Material)
public:
    enum class MaterialType {
        None, Phong
    };

protected:
    MaterialType matType;

public:
    Material(MaterialType _matType)
        : matType(_matType) {}

    Material(const Material& material)
        : matType(material.matType) {}

    Material(Material&& material)
        : matType(material.matType) {}

    inline const MaterialType& getType() const {
        return matType;
    }

    Material() = default;
    virtual ~Material() = default;
};

class BasicMaterial: public Material {
    GENERATE_PTR(BasicMaterial)
private:
    glm::vec3 m_ObjColor;

public:

    BasicMaterial(glm::vec3 obj_color = glm::vec3(1.0)):
        m_ObjColor{obj_color} {}

};

class PhongMaterial: public Material {
    GENERATE_PTR(PhongMaterial)
private:
    glm::vec3 m_Diffuse;
    glm::vec3 m_Specular;

    float m_Shininess;
public:

    PhongMaterial(float shininess = 32): m_Shininess{shininess} {}

    PhongMaterial(
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        float shininess = 32
    ) : m_Diffuse{diffuse}, m_Specular{specular}, m_Shininess{shininess} {}


    inline void setDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse; }
    inline const glm::vec3& getDiffuse() const { return m_Diffuse; }

    inline void setSpecular(const glm::vec3& specular) { m_Specular = specular; }
    inline const glm::vec3& getSpecular() const { return m_Specular; }

    inline void setShininess(const float shininess) { m_Shininess = shininess; }
    inline float getShininess() const { return m_Shininess; }
};

#endif
