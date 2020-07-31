#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;
uniform float size;

void main()
{
  //float scale = size;
    TexCoords = vertex.zw;
    ParticleColor = color;
    //gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
    gl_Position = projection * vec4((vertex.xy * size) + offset, 0.0, 1.0);
}
