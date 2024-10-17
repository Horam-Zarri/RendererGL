
#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct DirLight {
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
uniform DirLight light;


void main() {

    vec3 obj_color = vec3(1.0, 1.0, 1.0);
    // ambient
    vec3 ambient = light.ambient * obj_color;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * obj_color;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * obj_color;

    vec3 result = ambient + diffuse + specular;
    //FragColor = texture(material.texture_specular1, TexCoords);
    FragColor = vec4(result, 1.0);
}
