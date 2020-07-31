#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;

void main()
{
  //color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
  color = spriteColor * texture(image, TexCoords);
  //color = texture(image, TexCoords);
}
