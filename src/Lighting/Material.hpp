#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>
#include "Texture/Texture.hpp"

#include "Core/Shader/Shader.hpp"
#include "Util/Ptr.hpp"
#include <variant>

enum class MaterialType {
    None, Solid, Phong
};

class Material {
    GENERATE_PTR(Material)
public:

protected:
    MaterialType matType;

    Material(MaterialType _matType)
    : matType(_matType) {}
public:

    Material(const Material& material)
    : matType(material.matType) {}

    Material(Material&& material)
    : matType(material.matType) {}

    inline const MaterialType& getType() const {
        return matType;
    }

    virtual ~Material() = default;
};

class BasicMaterial: public Material {
    GENERATE_PTR(BasicMaterial)
private:
    glm::vec3 m_ObjColor;

public:

    BasicMaterial(glm::vec3 obj_color = glm::vec3(1.0)):
        Material(MaterialType::Solid), m_ObjColor{obj_color} {}

    inline void setObjColor(const glm::vec3& obj_color) { m_ObjColor = obj_color; }
    inline const glm::vec3& getObjColor() const { return m_ObjColor; }

};


#endif
