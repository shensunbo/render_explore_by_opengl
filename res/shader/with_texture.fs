#version 300 es
precision highp float;

out vec4 FragColor;

layout (std140) uniform MaterialBlock {
    vec4 materialDiffuseColor;
    vec4 materialSpecularColor;
    float Opacity;
    float Shininess;
    float ShininessStrength;
} material;

in vec3 Normal;
in vec3 viewDir;
in vec2 TexCoords;
in vec3 lightDir;
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

uniform bool texture_diffuse_load;
uniform bool texture_specular_load;
uniform bool texture_normal_load;
uniform bool texture_ao_load;
uniform bool texture_alpha_load;
uniform bool texture_roughness_load;
uniform bool texture_metallic_load;

uniform samplerCube cubemap;
uniform mat4 cubemapRotateMatrix;

uniform vec3 viewPosition;

// x: p-right, n-left
// y: p-back, n-front
// z: p-top, n-bottom
vec3 lightPos = vec3(0.0, -1.0, 1.0);
vec3 specularColor = vec3(1.0);

// light attenuation
float constant = 1.0;
float linear = 0.09;
float quadratic = 0.032;

void main()
{    
    float diffCoef = 1.0;
    float roughness = -1.0;
    float metallic = -1.0;
    float realShininess = material.Shininess;
    vec3 realMaterialDiffuseColor = material.materialDiffuseColor.xyz;
    vec3 realMaterialSpecularColor = material.materialSpecularColor.xyz;
    vec3 realAmbientColor = vec3(0.1);
    vec3 finalColor = vec3(1.0);
    vec3 realNormal = normalize(Normal);

    // handle texture
    if(textureLoad){
        vec2 index = vec2(TexCoords.x, TexCoords.y);

        if(texture_diffuse_load){
            realMaterialDiffuseColor = texture(texture_diffuse, index).rgb;
        }

        if(texture_specular_load){
            realShininess = texture(texture_specular, index).r * 128.0;
        }

        if(texture_ao_load){
            diffCoef = texture(texture_ao, index).r;
        }

        if(texture_normal_load){
            realNormal = texture(texture_normal, TexCoords).rgb;
            realNormal = normalize(realNormal * 2.0 - 1.0);
            realNormal = normalize(TBN * realNormal);
        }

        if(texture_roughness_load){
            roughness = texture(texture_roughness, index).r;
        }

        if(texture_metallic_load){
            metallic = texture(texture_metallic, index).r;
        }
    }

    // realMaterialDiffuseColor = (1.0 + roughness * 0.2) * realMaterialDiffuseColor;
    vec3 a_normal = realNormal;

    // cal light effect 
    vec3 lightDir =normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPosition -  FragPos);

    // light attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    float diff = max(dot(a_normal, lightDir), 0.0);
    vec3 diffuse = realMaterialDiffuseColor * diff;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(a_normal, halfwayDir), 0.0), realShininess);
    vec3 specular = (spec * specularColor);
    // specular = specular * (1.0 - roughness);
    // vec3 specular = (spec * lightColor );

    vec3 envMapColor;
    // the first param of reflect should point from light to surface 
    vec3 reflectDir = reflect(-viewDir, a_normal);
    reflectDir =  (cubemapRotateMatrix * vec4(reflectDir, 1.0)).xyz;
    envMapColor = texture(cubemap, reflectDir).rgb;
    realAmbientColor = envMapColor * 0.5f;

    // TODO: mix specular with realAmbientColor will get error and show nothing, 
    // error code: gl error: 0x502
    // vec3 resultColor = mix(diffuse + specular, envMapColor, 0.1);

    // TODO: realAmbientColor should be multiplied or be added with diffuse
    // with roughness
    // vec3 resultColor = diffuse  + (specular + realAmbientColor * realMaterialDiffuseColor) * (1.0 - roughness);
    vec3 resultColor = diffuse * attenuation  + (specular * attenuation + realAmbientColor * realMaterialDiffuseColor) * (1.0 - roughness);
    // vec3 resultColor = diffuse  + (specular) * (1.0 - roughness);


    // basic
    // vec3 resultColor = diffuse  + (specular + realAmbientColor);
    resultColor = diffCoef * resultColor;

    // 测试镜面反射
    // resultColor = specular;

    // 测试AO贴图效果
    // resultColor = vec3(diffCoef);

    // 测试roughness
    //  resultColor = vec3( (1.0 - roughness));
    // resultColor = (specular + realAmbientColor) * (1.0 - roughness);

    // 测试metallic
    // resultColor = vec3(metallic);

    


    FragColor = vec4(resultColor, 1.0);
}