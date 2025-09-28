#version 300 es
precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 lightDir;
out vec3 viewDir;
out vec3 FragPos;

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

    Normal = normalize(vec3(model * vec4(aNormal, 0.0)));
    FragPos = vec3(model * vec4(aPos, 1.0));
    // TODO: use CPU to calculate the viewPosition
    mat4 invLook = inverse(look);
    vec3 viewPosition = vec3(invLook[3]);

    lightDir =normalize(lightPos - FragPos);
    viewDir = normalize(viewPosition -  FragPos);

    TexCoords = aTexCoords;    
    gl_Position = projection * look * view * model * vec4(aPos, 1.0);
}