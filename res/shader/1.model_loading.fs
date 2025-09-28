#version 300 es
precision highp float;
out vec4 FragColor;

in vec3 Normal;
in vec3 viewDir;
in vec2 TexCoords;

uniform samplerCube cubemap;

void main()
{        
    vec3 a_normal = normalize(Normal);

    vec3 materialDiffuseColor = vec3(0.7,0.7,0.7);
    vec3 lightDir = vec3(0.0, 0.0, 1.0);
    vec3 reflectDir = reflect(-lightDir, a_normal);
    float diff = max(dot(a_normal, lightDir), 0.0);
    vec3 diffuse = materialDiffuseColor * diff;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(a_normal, halfwayDir), 0.0), 256);
    vec3 specular = (spec * materialDiffuseColor );

    vec3 R = reflect(viewDir, a_normal);
    R =  vec4(R, 1.0).xyz;
    vec3 envMapColor = texture(cubemap, R).rgb;

    vec3 resultColor = mix(diffuse + specular, materialDiffuseColor, 0.8);
    resultColor = mix(resultColor, envMapColor, 0.5);

    FragColor = vec4(resultColor, 1.0);
}