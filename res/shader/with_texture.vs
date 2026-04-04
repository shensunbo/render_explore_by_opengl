#version 300 es
precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;
// Per-instance model matrix (occupies locations 5-8)
layout(location = 5) in mat4 aInstanceModel;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 uVP; // projection * view

void main()
{   
    FragPos = vec3(aInstanceModel * vec4(aPos, 1.0));

    vec3 T = normalize(vec3(aInstanceModel * vec4(a_tangent, 0.0)));
    vec3 B = normalize(vec3(aInstanceModel * vec4(a_bitangent, 0.0)));
    vec3 N = normalize(vec3(aInstanceModel * vec4(aNormal, 0.0)));

    TBN = mat3(T, B, N);
    Normal = N;

    TexCoords = aTexCoords;    
    gl_Position = uVP * vec4(FragPos, 1.0);
}