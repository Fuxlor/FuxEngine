#version 330 core

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_SPOT_LIGHTS 8

in vec3 FragPos;
in vec3 Normal;
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float innerCutoff;
    float outerCutoff;
    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform int directionalLightCount;

uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int spotLightCount;

uniform vec3 viewPosition;
uniform float ambientStrength;
uniform sampler2D ourTexture;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDirection, vec3 textureColor)
{
    vec3 lightDirection = normalize(light.position - FragPos);

    float distance = length(light.position - FragPos);

    float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
    );

    float diffuse = max(dot(normal, lightDirection), 0.0);

    vec3 reflectDirection = reflect(-lightDirection, normal);

    float specular = pow(
        max(dot(viewDirection, reflectDirection), 0.0),
        32.0
    );

    vec3 diffuseColor = textureColor * light.color * light.intensity * diffuse;
    vec3 specularColor = light.color * light.intensity * specular;

    return (diffuseColor + specularColor) * attenuation;
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection, vec3 textureColor)
{
    vec3 lightDirection = normalize(-light.direction);

    float diffuse = max(dot(normal, lightDirection), 0.0);

    vec3 reflectDirection = reflect(-lightDirection, normal);

    float specular = pow(
        max(dot(viewDirection, reflectDirection), 0.0),
        32.0
    );

    vec3 diffuseColor = textureColor * light.color * light.intensity * diffuse;
    vec3 specularColor = light.color * light.intensity * specular;

    return diffuseColor + specularColor;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection, vec3 textureColor)
{
    vec3 lightDirection = normalize(light.position - FragPos);

    float distance = length(light.position - FragPos);

    float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
    );

    float theta = dot(lightDirection, normalize(-light.direction));

    float epsilon = light.innerCutoff - light.outerCutoff;

    float intensity = clamp(
        (theta - light.outerCutoff) / epsilon,
        0.0,
        1.0
    );

    float diffuse = max(dot(normal, lightDirection), 0.0);

    vec3 reflectDirection = reflect(-lightDirection, normal);

    float specular = pow(
        max(dot(viewDirection, reflectDirection), 0.0),
        32.0
    );

    vec3 diffuseColor = textureColor * light.color * light.intensity * diffuse;
    vec3 specularColor = light.color * light.intensity * specular;

    return (diffuseColor + specularColor) * attenuation * intensity;
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(viewPosition - FragPos);
    vec3 textureColor = texture(ourTexture, TexCoord).rgb;

    vec3 finalColor = textureColor * ambientStrength;

    for (int i = 0; i < pointLightCount; i++)
    {
        finalColor += CalculatePointLight(
            pointLights[i],
            normal,
            viewDirection,
            textureColor
        );
    }

    for (int i = 0; i < directionalLightCount; i++)
    {
        finalColor += CalculateDirectionalLight(
            directionalLights[i],
            normal,
            viewDirection,
            textureColor
        );
    }

    for (int i = 0; i < spotLightCount; i++)
    {
        finalColor += CalculateSpotLight(
            spotLights[i],
            normal,
            viewDirection,
            textureColor
        );
    }

    FragColor = vec4(finalColor, 1.0);
}