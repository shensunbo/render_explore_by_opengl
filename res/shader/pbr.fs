#version 300 es
precision highp float;

out vec4 FragColor;

layout (std140) uniform MaterialBlock {
    vec4 materialDiffuseColor;
    vec4 materialSpecularColor;
    float Opacity;
    float Shininess;
    float ShininessStrength;
    int texture_diffuse_load;
    int texture_specular_load;
    int texture_normal_load;
    int texture_ao_load;
    int texture_alpha_load;
    int texture_roughness_load;
    int texture_metallic_load;
    int texture_emissive_load;
} material;

in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;
in vec3 FragPos;

uniform bool textureLoad;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D texture_ao;
uniform sampler2D texture_alpha;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metallic;

uniform bool enableDiffuseTex;
uniform bool enableSpecularTex;
uniform bool enableNormalTex;
uniform bool enableAOTex;
uniform bool enableRoughnessTex;
uniform bool enableMetallicTex;

uniform samplerCube cubemap;
uniform float exposure;
uniform mat4 cubemapRotateMatrix;

uniform vec3 viewPosition;

const float PI = 3.14159265359;

// x: p-right, n-left
// y: p-back, n-front
// z: p-top, n-bottom
vec3 lightPos = vec3(0.0, -1.0, 1.0);
vec3 lightColor = vec3(1.0);

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom, 0.0001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(texture_normal, TexCoords).xyz * 2.0 - 1.0;
    vec3 worldNormal = normalize(TBN * tangentNormal);
    return worldNormal;
}

void main() {
    vec3 albedo = material.materialDiffuseColor.rgb;
    float metallic = 0.0;
    float roughness = 0.5;
    float ao = 1.0;
    vec3 N = normalize(Normal);

    if (textureLoad) {
        if (material.texture_diffuse_load != 0 && enableDiffuseTex) {
            albedo = texture(texture_diffuse, TexCoords).rgb;
        }
        if (material.texture_normal_load != 0 && enableNormalTex) {
            N = getNormalFromMap();
        }
        if (material.texture_ao_load != 0 && enableAOTex) {
            ao = texture(texture_ao, TexCoords).r;
        }
        if (material.texture_roughness_load != 0 && enableRoughnessTex) {
            roughness = texture(texture_roughness, TexCoords).r;
        } else if (material.texture_specular_load != 0 && enableSpecularTex) {
            roughness = 1.0 - clamp(texture(texture_specular, TexCoords).r, 0.0, 1.0);
        }
        if (material.texture_metallic_load != 0 && enableMetallicTex) {
            metallic = texture(texture_metallic, TexCoords).r;
        }
    }

    roughness = clamp(roughness, 0.04, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);

    vec3 V = normalize(viewPosition - FragPos);
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    vec3 radiance = lightColor * attenuation;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 numerator = NDF * G * F;
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denom;

    float NdotL = max(dot(N, L), 0.0);

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    // Simple ambient from environment map
    vec3 R = reflect(-V, N);
    R = (cubemapRotateMatrix * vec4(R, 1.0)).xyz;
    vec3 envColor = texture(cubemap, R).rgb;
    vec3 ambient = envColor * albedo * ao * 0.3;

    vec3 color = ambient + Lo;
    // HDR tonemapping-ish via exposure multiplier
    color = color * exposure;

    FragColor = vec4(color, material.Opacity);
}
