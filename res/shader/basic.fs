#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 viewDir;
in vec2 TexCoords;
in vec3 lightDir;

uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float Opacity;
uniform float Shininess;
uniform float ShininessStrength;

uniform samplerCube cubemap;

void main()
{    
    vec3 ambientColor = materialDiffuseColor * 1.5;

    vec3 lightColor = vec3(1.0);

    vec3 a_normal = normalize(Normal);

    vec3 reflectDir = reflect(-lightDir, a_normal);
    float diff = max(dot(a_normal, lightDir), 0.0);
    vec3 diffuse = materialDiffuseColor * diff;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(a_normal, halfwayDir), 0.0), Shininess);
    vec3 specular = (spec * materialSpecularColor );
    // vec3 specular = (spec * lightColor );

    vec3 envMapColor;
    vec3 ambientReflection;
    vec3 reflectionVector = reflect(viewDir, a_normal);
    envMapColor = texture(cubemap, reflectionVector).rgb;

    vec3 resultColor = mix(diffuse + specular, envMapColor, 0.3);
    // vec3 resultColor = diffuse + specular + envMapColor * 0.3;

    FragColor = vec4(resultColor, Opacity);
}