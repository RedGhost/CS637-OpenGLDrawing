#version 430

uniform mat4 modelView;
uniform mat4 projection;
uniform vec4 lightPosition;
uniform vec4 eyePosition;

in vec3 vPoint;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 fN;
out vec3 fE;
out vec3 fL;

out vec2 texCoord;

void
main()
{

    fN = vNormal;
    fE = (eyePosition - vec4(vPoint, 1.0f)).xyz;

    if(lightPosition.w != 0.0) {
        fL = lightPosition.xyz - vPoint;
    }
    else {
        fL = lightPosition.xyz;
    }

    texCoord = vTexCoord;

    gl_Position = projection * modelView * vec4(vPoint, 1.0f);
}
