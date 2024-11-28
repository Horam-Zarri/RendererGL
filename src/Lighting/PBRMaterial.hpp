#ifndef PBR_MATERIAL_H
#define PBR_MATERIAL_H

#include "Lighting/Material.hpp"
#include "Util/Ptr.hpp"

class PBRMaterial : public Material {
    GENERATE_PTR(PBRMaterial)
private:
    glm::vec3 m_Albedo;
    float m_Roughness;
    float m_Metallic;
    float m_Ao;
public:

    PBRMaterial() :
        Material(MaterialType::PBR),
        m_Albedo{1.f}, m_Roughness{0.f},
        m_Metallic{0.f}, m_Ao{1.f}
    {}

    PBRMaterial(
        const glm::vec3& albedo,
        float roughness,
        float metallic,
        float ao
    ) : Material(MaterialType::PBR),
        m_Albedo{albedo}, m_Roughness{roughness},
        m_Metallic{metallic}, m_Ao{ao}
    {}

    inline void setAlbedo(const glm::vec3& albedo) { m_Albedo = albedo; }
    inline const glm::vec3& getAlbedo() const { return m_Albedo; }

    inline void setRoughness(float roughness) { m_Roughness = roughness; }
    inline float getRoughness() const { return m_Roughness; }

    inline void setMetallic(float metallic) { m_Metallic = metallic; }
    inline float getMetallic() const { return m_Metallic; }

    inline void setAo(float ao) { m_Ao = ao; }
    inline float getAo() const { return m_Ao; }
};

#endif
