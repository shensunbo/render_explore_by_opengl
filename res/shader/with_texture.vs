#version 330 core
precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out vec2 TexCoords;
out vec3 Normal;
// out vec3 lightDir;
// out vec3 viewDir;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 look;

void main()
{   
    // x: p-bottom, n-top
    // y: p-left, n-right
    // z: p-back, n-front
    vec3 lightPos = vec3(0.0, 1.0, 1.0);

    mat3 normalMatrix = transpose(mat3(model)); 
    Normal = normalMatrix * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    mat4 invLook = inverse(look);
    // vec3 viewPosition = vec3(invLook[3]);

    // lightDir =normalize(lightPos - FragPos);
    // viewDir = normalize(viewPosition -  FragPos);

    vec3 T = normalize(vec3(model * vec4(a_tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(a_bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));

    TBN = mat3(T, B, N);

    TexCoords = aTexCoords;    
    gl_Position = projection * look * view * model * vec4(aPos, 1.0);
}