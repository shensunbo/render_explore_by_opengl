#version 300 es
precision highp float;
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 uMVP;
void main()
{
    TexCoords = aPos;
    vec4 pos = uMVP * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}