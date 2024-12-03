#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube envMap;

void main()
{
    FragColor = texture(envMap, TexCoords);
}
