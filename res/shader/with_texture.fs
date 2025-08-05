#version 330 core
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

uniform bool texture_diffuse_load;
uniform bool texture_specular_load;
uniform bool texture_normal_load;
uniform bool texture_ao_load;
uniform bool texture_alpha_load;

uniform samplerCube cubemap;
uniform mat4 cubemapRotateMatrix;

uniform vec3 viewPosition;

// x: p-right, n-left
// y: p-back, n-front
// z: p-top, n-bottom
vec3 lightPos = vec3(0.0, -1.0, 1.0);

void main()
{    
    float diffCoef = 1.0;
    vec3 realMaterialDiffuseColor = material.materialDiffuseColor.xyz;
    vec3 realMaterialSpecularColor = material.materialSpecularColor.xyz;
    vec3 realAmbientColor = vec3(0.1);
    vec3 finalColor = vec3(1.0);
    vec3 realNormal = normalize(Normal);;

    // handle texture
    if(textureLoad){
        vec2 index = vec2(TexCoords.x, TexCoords.y);

        if(texture_diffuse_load){
            realMaterialDiffuseColor = texture(texture_diffuse, index).rgb;
        }

        if(texture_specular_load){
            realMaterialSpecularColor = vec3(texture(texture_specular, index).r);
        }

        if(texture_ao_load){
            diffCoef = texture(texture_ao, index).r;
        }

        if(texture_normal_load){
            realNormal = texture(texture_normal, TexCoords).rgb;
            realNormal = normalize(realNormal * 2.0 - 1.0);
            realNormal = normalize(TBN * realNormal);
        }
    }

    // realMaterialDiffuseColor = diffCoef * realMaterialDiffuseColor;
    vec3 a_normal = realNormal;

    // cal light effect 
    vec3 lightDir =normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPosition -  FragPos);

    float diff = max(dot(a_normal, lightDir), 0.0);
    vec3 diffuse = realMaterialDiffuseColor * diff;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(a_normal, halfwayDir), 0.0), 25);
    vec3 specular = (spec * realMaterialSpecularColor);
    // vec3 specular = (spec * lightColor );

    vec3 envMapColor;
    // the first param of reflect should point from light to surface 
    vec3 reflectDir = reflect(-viewDir, a_normal);
    reflectDir =  (cubemapRotateMatrix * vec4(reflectDir, 1.0)).xyz;
    envMapColor = texture(cubemap, reflectDir).rgb;
    realAmbientColor = envMapColor * 0.2f;

    // TODO: mix specular with realAmbientColor will get error and show nothing, 
    // error code: gl error: 0x502
    // vec3 resultColor = mix(diffuse + specular, envMapColor, 0.1);
    vec3 resultColor = diffuse  + specular + realAmbientColor;
    resultColor = diffCoef * resultColor;
    // vec3 resultColor = diffuse  + realAmbientColor;
    // resultColor = clamp(diffuse + specular + realAmbientColor, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

    // resultColor = mix(resultColor, realAmbientColor, 0.1);

    FragColor = vec4(resultColor, 1.0);
}