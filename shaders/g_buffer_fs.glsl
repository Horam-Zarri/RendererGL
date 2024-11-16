#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;

uniform bool hasDiffuse;
uniform bool hasSpecular;
uniform bool hasNormal;


struct MaterialSolid {
    // vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // float shininess;
};

struct MaterialTexture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    // sampler2D shadow;
};

uniform MaterialSolid material;
uniform MaterialTexture materialMaps;

uniform bool hasNormal;

void main()
{
    gPosition = FragPos;

    if (hasNormal) {
        gNormal = texture(materialMaps.normal, fs_in.TexCoords).rgb;
        gNormal = gNormal * 2.0 - 1.0;
        gNormal = normalize(fs_in.TBN * gNormal);
    } else
        gNormal = normalize(Normal);

    if (hasDiffuse)
        gAlbedoSpec.rgb = texture(materialMaps.diffuse, TexCoords).rgb;
    else
        gAlbedoSpec.rgb = material.diffuse;

    if (hasSpecular)
        gAlbedoSpec.a = texture(materialMaps.specular, TexCoords).r;
    else
        gAlbedoSpec.a = dot(material.specular, vec3(1.0)) / 3.0;
}
