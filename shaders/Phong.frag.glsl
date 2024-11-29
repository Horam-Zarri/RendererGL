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
    sampler2D normal;
};

struct DeferredTexture {
    sampler2D gPosition;
    sampler2D gNormal;
    sampler2D gAlbedoSpec;
    sampler2D gPositionLightSpace;
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
#define NR_SPOT_LIGHTS 10

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;



uniform vec3 viewPos;

uniform MaterialSolid material;
uniform MaterialTexture materialMaps;
uniform DeferredTexture deferredMaps;

uniform bool deferred;

uniform DirectionalLight directionalLight;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform int pointLightsSize;
uniform int spotLightsSize;

uniform bool hasDiffuse;
uniform bool hasSpecular;
uniform bool hasShadow;
uniform bool hasNormal;

uniform bool blinn;
uniform float bloomLevel=1.2f;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

vec3 CalcAmbient(vec3 lightAmbient)
{
    vec3 ambient;
    if (deferred) {
        ambient = lightAmbient * texture(deferredMaps.gAlbedoSpec, fs_in.TexCoords).rgb;
    } else if (hasDiffuse) {
        ambient = lightAmbient * vec3(texture(materialMaps.diffuse, fs_in.TexCoords));
    } else {
        ambient = lightAmbient * material.ambient;
    }
    return ambient;
}

vec3 CalcDiffuse(vec3 lightDiffuse, float diff)
{
    vec3 diffuse;
    if (deferred) {
        diffuse = lightDiffuse * diff * texture(deferredMaps.gAlbedoSpec, fs_in.TexCoords).rgb;
    } else if (hasDiffuse) {
        diffuse = lightDiffuse * diff * vec3(texture(materialMaps.diffuse, fs_in.TexCoords));
    } else {
        diffuse = lightDiffuse * diff * material.diffuse;
    }
    return diffuse;
}

vec3 CalcSpecular(vec3 lightSpecular, float spec)
{
    vec3 specular;
    if (deferred && hasSpecular) {
        specular = lightSpecular * spec * texture(deferredMaps.gAlbedoSpec, fs_in.TexCoords).a;
    } else if (hasDiffuse && hasSpecular) {
        specular = lightSpecular * spec * vec3(texture(materialMaps.specular, fs_in.TexCoords));
    } else if (hasSpecular) {
        specular = lightSpecular * spec * material.specular;
    }
    return specular;
}

void main() {

    vec3 _FragPos, _Normal;

    if (deferred) {
        _FragPos = texture(deferredMaps.gPosition, fs_in.TexCoords).rgb;
        _Normal = texture(deferredMaps.gNormal, fs_in.TexCoords).rgb;
    } else {
        _FragPos = fs_in.FragPos;
        if (hasNormal) {
            _Normal = texture(materialMaps.normal, fs_in.TexCoords).rgb;
            _Normal = _Normal * 2.0 - 1.0;
            _Normal = normalize(fs_in.TBN * _Normal);
        } else
            _Normal = normalize(fs_in.Normal);
    }

    vec3 viewDir = normalize(viewPos - _FragPos);

    vec3 result = CalcDirLight(directionalLight, _Normal, viewDir);

    // phase 2: point lights
    for(int i = 0; i < pointLightsSize; i++)
        result += CalcPointLight(pointLights[i], _Normal, _FragPos, viewDir);

    // phase 3: spot lights
    for(int i = 0; i < spotLightsSize; i++)
        result += CalcSpotLight(spotLights[i], _Normal, _FragPos, viewDir);

    FragColor = vec4(result, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    BrightColor =
        brightness > bloomLevel ?
        vec4(FragColor.rgb, 1.0) :
        vec4(0.0, 0.0, 0.0, 1.0);
}


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

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
    // combine results

    vec3 ambient = CalcAmbient(light.ambient);
    vec3 diffuse = CalcDiffuse(light.diffuse, diff);
    vec3 specular = CalcSpecular(light.specular, spec);

    vec3 lighting;

    if (hasShadow) {
        vec4 _FragPosLightSpace =
            deferred ?
            texture(deferredMaps.gPositionLightSpace, fs_in.TexCoords) :
            fs_in.FragPosLightSpace;

        float shadow = ShadowCalculation(_FragPosLightSpace, normal, lightDir);
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

    vec3 ambient = CalcAmbient(light.ambient);
    vec3 diffuse = CalcDiffuse(light.diffuse, diff);
    vec3 specular = CalcSpecular(light.specular, spec);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results

    vec3 ambient = CalcAmbient(light.ambient);
    vec3 diffuse = CalcDiffuse(light.diffuse, diff);
    vec3 specular = CalcSpecular(light.specular, spec);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {

    // not required for ortho, but do it anyways
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    float closestDepth = texture(materialMaps.shadow, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(materialMaps.shadow, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(materialMaps.shadow, projCoords.xy + vec2(x, y) *
                                     texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;

}
