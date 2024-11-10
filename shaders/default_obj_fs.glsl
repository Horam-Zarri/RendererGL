#version 330 core

struct MaterialSolid {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct MaterialTexture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D shadow;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 10

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;



uniform vec3 viewPos;

uniform MaterialSolid material;
uniform MaterialTexture materialMaps;

uniform DirectionalLight directionalLight;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int pointLightsSize;

uniform bool hasDiffuse;
uniform bool hasSpecular;
uniform bool hasShadow;

uniform bool blinn;
//uniform SpotLight spotLight;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec4 fragPosLightSpace);

void main() {

    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 result = CalcDirLight(directionalLight, norm, viewDir);

    // phase 2: point lights
    for(int i = 0; i < pointLightsSize; i++)
        result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
    // phase 3: spot light
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);

    if (hasShadow)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;

    if (blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    // combine results

    vec3 ambient, diffuse, specular;

    if (hasDiffuse) {
        ambient = light.ambient * vec3(texture(materialMaps.diffuse, fs_in.TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(materialMaps.diffuse, fs_in.TexCoords));

        if (hasSpecular)
            specular = light.specular * spec * vec3(texture(materialMaps.specular, fs_in.TexCoords));
    } else {
        // assume diffuseless material also doesnt come with specular
        ambient = light.ambient * material.ambient;
        diffuse = light.diffuse * diff * material.diffuse;
        specular = light.specular * spec * material.specular;
    }


    vec3 lighting;

    if (hasShadow) {
        float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
        lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    } else {
        lighting = ambient + diffuse + specular;
    }

    return lighting;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = 0.0;
    if (blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results

    vec3 ambient, diffuse, specular;

    if (hasDiffuse) {
        ambient = light.ambient * vec3(texture(materialMaps.diffuse, fs_in.TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(materialMaps.diffuse, fs_in.TexCoords));

        if (hasSpecular)
            specular = light.specular * spec * vec3(texture(materialMaps.specular, fs_in.TexCoords));

    } else {
        ambient = light.ambient * material.ambient;
        diffuse = light.diffuse * diff * material.diffuse;
        specular = light.specular * spec * material.specular;
    }

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    // attenuation
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//    // spotlight intensity
//    float theta = dot(lightDir, normalize(-light.direction));
//    float epsilon = light.cutOff - light.outerCutOff;
//    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//    // combine results
//    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, fs_in.TexCoords));
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse, fs_in.TexCoords));
//    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular, fs_in.TexCoords));
//    ambient *= attenuation * intensity;
//    diffuse *= attenuation * intensity;
//    specular *= attenuation * intensity;
//    return (ambient + diffuse + specular);
//}

float ShadowCalculation(vec4 fragPosLightSpace) {

    // not required for ortho, but do it anyways
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(materialMaps.shadow, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}
