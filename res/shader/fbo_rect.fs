#version 300 es
precision highp float;
in vec2 TexCoords;
out vec4 color;

uniform sampler2D fboTexture;

void main()
{
    color = texture(fboTexture, TexCoords);
    // color = vec4(vec3(1.0 - texture(fboTexture, TexCoords)), 1.0);
}