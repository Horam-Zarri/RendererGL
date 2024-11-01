#include "material.hpp"


PhongMaterial::PhongMaterial(Texture diffuse_texture):
    m_Diffuse{diffuse_texture}, m_HasSpecular{false},
    m_UseTexture{true} {}

PhongMaterial::PhongMaterial(Texture diffuse_texture, Texture specular_texture):
    m_Diffuse{diffuse_texture}, m_Specular{specular_texture},
    m_HasSpecular{true}, m_UseTexture{true} {}

PhongMaterial::PhongMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
    m_Ambient{ambient}, m_Diffuse{diffuse}, m_Specular{specular},
    m_UseTexture{false} {}

PhongMaterial::PhongMaterial(glm::vec3 obj_color):
    m_Ambient{obj_color}, m_Diffuse{obj_color}, m_Specular{obj_color},
    m_UseTexture{false} {}


void PhongMaterial::send_uniforms(Shader& shader)  {
    if (m_UseTexture) {
        shader.setFloat("material.texture_diffuse", 0);
        shader.setFloat("material.texture_specular", 1);
        shader.setFloat("material.shininess", 32);

        std::get<Texture>(m_Diffuse).bind(0);
        if (m_HasSpecular)
            std::get<Texture>(m_Specular).bind(1);
    } else {
        shader.setVec3("material.ambient", std::get<glm::vec3>(m_Ambient));
        shader.setVec3("material.diffuse", std::get<glm::vec3>(m_Diffuse));
        shader.setVec3("material.specular", std::get<glm::vec3>(m_Specular));
    }
}
