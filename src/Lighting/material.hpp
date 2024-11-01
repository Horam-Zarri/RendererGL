#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>
#include "Texture/texture.hpp"

#include "Core/Shader/shader.hpp"
#include <variant>

class Material {
public:
    virtual void send_uniforms(Shader& shader) = 0;
};


class PhongMaterial: public Material {
private:

    using MaterialProperty = std::variant<Texture, glm::vec3>;

    MaterialProperty m_Ambient;
    MaterialProperty m_Diffuse;
    MaterialProperty m_Specular;

    bool m_UseTexture;
    bool m_HasSpecular;
public:

    PhongMaterial(Texture diffuse_texture);
    PhongMaterial(Texture diffuse_texture, Texture specular_texture);
    PhongMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    PhongMaterial(glm::vec3 obj_color);

    virtual void send_uniforms(Shader& shader) override;
};

#endif
