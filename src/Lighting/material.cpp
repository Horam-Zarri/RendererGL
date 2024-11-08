#include "Material.hpp"




//void PhongMaterial::send_uniforms(Shader& shader)  {
//    if (m_UseTexture) {
//        shader.setFloat("material.texture_diffuse", 0);
//        shader.setFloat("material.texture_specular", 1);
//        shader.setFloat("material.shininess", 32);
//
//        std::get<Texture>(m_Diffuse).bind(0);
//        if (m_HasSpecular)
//            std::get<Texture>(m_Specular).bind(1);
//    } else {
//        shader.setVec3("material.ambient", std::get<glm::vec3>(m_Ambient));
//        shader.setVec3("material.diffuse", std::get<glm::vec3>(m_Diffuse));
//        shader.setVec3("material.specular", std::get<glm::vec3>(m_Specular));
//    }
//}
