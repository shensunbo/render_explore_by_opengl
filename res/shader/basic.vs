#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 viewDir;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 look;

void main()
{   
    mat3 normalMatrix = transpose(mat3(model)); 
    Normal = normalMatrix * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    mat4 invLook = inverse(look);
    vec3 viewPosition = vec3(invLook[3]);
    viewDir = normalize(viewPosition -  FragPos);

    TexCoords = aTexCoords;    
    gl_Position = projection * look * view * model * vec4(aPos, 1.0);
}