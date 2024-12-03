#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    vec4 WorldPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.WorldPosLightSpace = lightSpaceMatrix * vec4(vs_out.WorldPos, 1.0);

    gl_Position = projection * view * vec4(vs_out.WorldPos, 1.0);
}
