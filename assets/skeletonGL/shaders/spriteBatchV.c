#version 330 core

layout (location = 0) in vec2 vertex; //vec2 pos
layout (location = 1) in vec2 UVCoords;
layout (location = 3) in mat4 instanceModel; // Instanced array

out vec2 TexCoords;

/* uniform mat4 model; */
uniform mat4 projection;

void main()
{
    //NOTE, flip the y axis on the UV coords since they default to inverted anyway
    //UPDATE, this messed up custom UV values, DON'T
    TexCoords = vec2(UVCoords.x, UVCoords.y);
    //flips image veritcally, opengl flips teh y axis, fixed in ortho matrix
    gl_Position = projection * instanceModel * vec4(vertex.xy, 0.0, 1.0);
}

