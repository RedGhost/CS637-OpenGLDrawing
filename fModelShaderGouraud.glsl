#version 430

in vec4 color;
in vec3 texCoord;

out vec4 outColor;

uniform sampler3D myTexture;

void
main()
{
    outColor = color * texture( myTexture, texCoord );
}
