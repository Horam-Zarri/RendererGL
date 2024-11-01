#version 330 core

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main() {

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(directionalLight, norm, viewDir);

    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular, TexCoords));

    return (ambient + diffuse + specular);
}
