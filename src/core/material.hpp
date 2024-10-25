#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>
#include "core/texture.hpp"

#include "shader.hpp"
#include <variant>

class Material {
public:
    Material() {}
    virtual void send_uniforms(Shader& shader) = 0;
};


class PhongMaterial: public Material {
private:

    using MaterialProperty = std::variant<Texture, glm::vec3>;

    MaterialProperty m_Ambient;
    MaterialProperty m_Diffuse;
    MaterialProperty m_Specular;

    bool m_UseTexture;
public:

    PhongMaterial(Texture diffuse_texture, Texture specular_texture):
        m_Diffuse{diffuse_texture}, m_Specular{specular_texture}, m_UseTexture{true} {}

    PhongMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
        m_Ambient{ambient}, m_Diffuse{diffuse}, m_Specular{specular}, m_UseTexture{false} {}

    PhongMaterial(glm::vec3 obj_color): m_Ambient{obj_color}, m_Diffuse{obj_color},
        m_Specular{obj_color}, m_UseTexture{false} {}

    virtual void send_uniforms(Shader& shader) override {
        if (m_UseTexture) {
            shader.setFloat("material.texture_diffuse", 0);
            shader.setFloat("material.texture_specular", 1);
            std::get<Texture>(m_Diffuse).bind(0);
            std::get<Texture>(m_Specular).bind(1);
        } else {
            shader.setVec3("material.ambient", std::get<glm::vec3>(m_Ambient));
            shader.setVec3("material.diffuse", std::get<glm::vec3>(m_Diffuse));
            shader.setVec3("material.specular", std::get<glm::vec3>(m_Specular));
        }
    }
};

#endif
