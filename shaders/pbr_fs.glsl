#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
} fs_in;

struct MaterialSolid {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct MaterialTexture {
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
};

uniform bool gammaCorrect=true;

uniform bool hasAlbedo;
uniform bool hasMetallic;
uniform bool hasRoughness;
uniform bool hasAo;
uniform bool hasNormal;

#define NR_MAX_LIGHTS 10

struct PointLight {
    vec3 position;
    vec3 color;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform PointLight pointLights[NR_MAX_LIGHTS];
uniform int pointLightsSize;
uniform DirectionalLight directionalLight;

uniform MaterialSolid material;
uniform MaterialTexture materialMaps;


uniform vec3 camPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anyways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap(sampler2D normalMap)
{
    vec3 tangentNormal = texture(normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.WorldPos);
    vec3 Q2  = dFdy(fs_in.WorldPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalcPointLightRadiance(
    PointLight light,
    vec3 N,
    vec3 V,
    vec3 F0,
    vec3 albedo,
    float roughness,
    float metallic
) {
    vec3 L = normalize(light.position - fs_in.WorldPos);
    vec3 H = normalize(L + V);

    float distance = length(L);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 specular = (NDF * G * F) /
        (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
    return Lo;
}

vec3 CalcDirLightRadiance(
    DirectionalLight light,
    vec3 N,
    vec3 V,
    vec3 F0,
    vec3 albedo,
    float roughness,
    float metallic
) {
    vec3 L = normalize(-light.direction - fs_in.WorldPos);
    vec3 H = normalize(L + V);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 specular = (NDF * G * F) /
        (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    vec3 Lo = (kD * albedo / PI + specular) * light.color * NdotL;
    return Lo;
}

void main()
{
    vec3 _Albedo, _Normal;
    float _Metallic, _Roughness, _Ao;

    _Albedo = hasAlbedo ? texture(materialMaps.albedoMap, fs_in.TexCoords).rgb
        : material.albedo;
    _Normal = hasNormal ? getNormalFromMap(materialMaps.normalMap)
        : fs_in.Normal;
    _Metallic = hasMetallic ? texture(materialMaps.metallicMap, fs_in.TexCoords).r
        : material.metallic;
    _Roughness = hasRoughness ? texture(materialMaps.roughnessMap, fs_in.TexCoords).r
        : material.roughness;
    _Ao = hasAo ? texture(materialMaps.aoMap, fs_in.TexCoords).r
        : material.ao;

    //if (gammaCorrect && hasAlbedo) _Albedo = pow(_Albedo, vec3(2.2));

    vec3 N = normalize(_Normal);
    vec3 V = normalize(camPos - fs_in.WorldPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, _Albedo, _Metallic);

    vec3 Lo = vec3(0.0);

    //Lo += CalcDirLightRadiance(directionalLight, V, N, F0, _Albedo, _Roughness, _Metallic);

    for (int i = 0; i < pointLightsSize; i++)
        Lo += CalcPointLightRadiance(pointLights[i], V, N, F0, _Albedo, _Roughness, _Metallic);

    vec3 ambient = vec3(0.03) * _Albedo * _Ao;
    vec3 color = ambient + Lo;

    // Tone mapping and gamma correction is handled in postprocess
    // color = color / (color + vec3(1.0));
    // gamma correct
    // color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(_Roughness, 0.0, 0.0, 1.0);
}
