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
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;
uniform bool hasNormalMap;
uniform bool hasEmissiveMap;
uniform bool hasShadowMap;
uniform bool doubleSided;
uniform bool gammaCorrection;
uniform float materialOpacity;
uniform float emissiveStrength;
uniform float materialShininess;
uniform float materialSpecularStrength;

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

    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float specular = pow(
        max(dot(normal, halfwayDirection), 0.0),
        materialShininess
    );

    vec3 diffuseColor = textureColor * light.color * light.intensity * diffuse;
    vec3 specularColor = light.color * light.intensity * specular * materialSpecularStrength;

    return (diffuseColor + specularColor) * attenuation;
}

float CalculateShadow(vec3 normal, vec3 lightDirection)
{
    if (!hasShadowMap)
        return 0.0;

    vec4 lightSpacePosition = lightSpaceMatrix * vec4(FragPos, 1.0);
    vec3 projected = lightSpacePosition.xyz / lightSpacePosition.w;
    projected = projected * 0.5 + 0.5;
    if (projected.z > 1.0 || any(lessThan(projected.xy, vec2(0.0))) || any(greaterThan(projected.xy, vec2(1.0))))
        return 0.0;

    float bias = max(0.0005, 0.004 * (1.0 - dot(normal, lightDirection)));
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(shadowMap, projected.xy + vec2(x, y) * texelSize).r;
            shadow += projected.z - bias > closestDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection, vec3 textureColor, float shadow)
{
    vec3 lightDirection = normalize(-light.direction);

    float diffuse = max(dot(normal, lightDirection), 0.0);

    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float specular = pow(
        max(dot(normal, halfwayDirection), 0.0),
        materialShininess
    );

    vec3 diffuseColor = textureColor * light.color * light.intensity * diffuse;
    vec3 specularColor = light.color * light.intensity * specular * materialSpecularStrength;

    return (diffuseColor + specularColor) * (1.0 - shadow);
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

    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float specular = pow(
        max(dot(normal, halfwayDirection), 0.0),
        materialShininess
    );

    vec3 diffuseColor = textureColor * light.color * light.intensity * diffuse;
    vec3 specularColor = light.color * light.intensity * specular * materialSpecularStrength;

    return (diffuseColor + specularColor) * attenuation * intensity;
}

void main()
{
    vec3 normal = normalize(Normal);
    if (doubleSided && !gl_FrontFacing)
        normal = -normal;

    if (hasNormalMap)
    {
        vec3 positionDx = dFdx(FragPos);
        vec3 positionDy = dFdy(FragPos);
        vec2 texcoordDx = dFdx(TexCoord);
        vec2 texcoordDy = dFdy(TexCoord);
        vec3 tangent = normalize(positionDx * texcoordDy.y - positionDy * texcoordDx.y);
        vec3 bitangent = normalize(-positionDx * texcoordDy.x + positionDy * texcoordDx.x);
        vec3 tangentNormal = texture(normalMap, TexCoord).xyz * 2.0 - 1.0;
        normal = normalize(mat3(tangent, bitangent, normal) * tangentNormal);
    }

    vec3 viewDirection = normalize(viewPosition - FragPos);
    vec4 albedoSample = texture(ourTexture, TexCoord);
    vec3 textureColor = albedoSample.rgb;

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
        vec3 lightDirection = normalize(-directionalLights[i].direction);
        float shadow = i == 0 ? CalculateShadow(normal, lightDirection) : 0.0;
        finalColor += CalculateDirectionalLight(
            directionalLights[i],
            normal,
            viewDirection,
            textureColor,
            shadow
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

    if (hasEmissiveMap)
        finalColor += texture(emissiveMap, TexCoord).rgb * emissiveStrength;

    if (gammaCorrection)
        finalColor = pow(max(finalColor, vec3(0.0)), vec3(1.0 / 2.2));
    FragColor = vec4(finalColor, albedoSample.a * materialOpacity);
}
