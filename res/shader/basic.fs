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

uniform bool textureLoad;
uniform sampler2D texture_diffuse;
// uniform vec3 materialDiffuseColor;
// uniform vec3 materialSpecularColor;
// uniform float Opacity;
// uniform float Shininess;
// uniform float ShininessStrength;

uniform samplerCube cubemap;

void main()
{    
    vec3 ambientColor = material.materialDiffuseColor.xyz * 1.5;

    vec3 lightColor = vec3(1.0);

    vec3 a_normal = normalize(Normal);

    vec3 realMaterialDiffuseColor = vec3(1.0, 0.0, 0.0);

    if(textureLoad){
        vec2 index = vec2(TexCoords.x, TexCoords.y);
        realMaterialDiffuseColor = texture(texture_diffuse, index).rgb;
    }


    vec3 reflectDir = reflect(-lightDir, a_normal);
    float diff = max(dot(a_normal, lightDir), 0.0);
    vec3 diffuse = realMaterialDiffuseColor * diff;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(a_normal, halfwayDir), 0.0), material.Shininess);
    vec3 specular = (spec * material.materialSpecularColor.xyz );
    // vec3 specular = (spec * lightColor );

    vec3 envMapColor;
    vec3 ambientReflection;
    vec3 reflectionVector = reflect(viewDir, a_normal);
    envMapColor = texture(cubemap, reflectionVector).rgb;

    // vec3 resultColor = mix(diffuse + specular, envMapColor, 0.1);
    // vec3 resultColor = diffuse + specular;

    FragColor = vec4(realMaterialDiffuseColor, material.Opacity);
}