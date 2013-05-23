#version 430

uniform mat4 modelView;
uniform mat4 projection;

in vec3 vPoint;
in vec3 vColor;

out vec3 color;

void
main()
{
    color = vColor;
    gl_Position = projection * modelView * vec4( vPoint, 1.0f );
}
