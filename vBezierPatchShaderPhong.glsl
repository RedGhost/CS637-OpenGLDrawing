#version 430

uniform mat4 modelView;
uniform mat4 projection;
uniform vec4 lightPosition1;
uniform vec4 lightPosition2;
uniform vec4 lightPosition3;
uniform vec4 eyePosition;

in vec3 vPoint;
in vec3 vNormal;

out vec3 fN;
out vec3 fE;
out vec3 fL1;
out vec3 fL2;
out vec3 fL3;

void
main()
{

    fN = vNormal;
    fE = (eyePosition - vec4(vPoint, 1.0f)).xyz;

    if(lightPosition1.w != 0.0) {
        fL1 = lightPosition1.xyz - vPoint;
    }
    else {
        fL1 = lightPosition1.xyz;
    }

    if(lightPosition2.w != 0.0) {
        fL2 = lightPosition2.xyz - vPoint;
    }
    else {
        fL2 = lightPosition2.xyz;
    }

    if(lightPosition3.w != 0.0) {
        fL3 = lightPosition3.xyz - vPoint;
    }
    else {
        fL3 = lightPosition3.xyz;
    }

    gl_Position = projection * modelView * vec4(vPoint, 1.0f);
}
