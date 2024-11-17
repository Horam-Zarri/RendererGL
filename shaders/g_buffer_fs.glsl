#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gPositionLightSpace;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;

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

uniform bool hasDiffuse;
uniform bool hasSpecular;
uniform bool hasNormal;

void main()
{
    gPosition = fs_in.FragPos;
    gPositionLightSpace = fs_in.FragPosLightSpace;

    if (hasNormal) {
        gNormal = texture(materialMaps.normal, fs_in.TexCoords).rgb;
        gNormal = gNormal * 2.0 - 1.0;
        gNormal = normalize(fs_in.TBN * gNormal);
    } else
        gNormal = normalize(fs_in.Normal);

    if (hasDiffuse)
        gAlbedoSpec.rgb = texture(materialMaps.diffuse, fs_in.TexCoords).rgb;
    else
        gAlbedoSpec.rgb = material.diffuse;

    if (hasSpecular)
        gAlbedoSpec.a = texture(materialMaps.specular, fs_in.TexCoords).r;
    else
        gAlbedoSpec.a = 0.0;
}
