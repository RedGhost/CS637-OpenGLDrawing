#version 430

in vec3 fN;
in vec3 fL1;
in vec3 fL2;
in vec3 fL3;
in vec3 fE;

out vec4 outColor;

uniform mat4 modelView;
uniform vec3 diffuse1;
uniform vec3 ambient1;
uniform vec3 specular1;
uniform vec3 diffuse2;
uniform vec3 ambient2;
uniform vec3 specular2;
uniform vec3 diffuse3;
uniform vec3 ambient3;
uniform vec3 specular3;

uniform float shininess;

void
main()
{
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L1 = normalize(fL1);
    vec3 L2 = normalize(fL2);
    vec3 L3 = normalize(fL3);

    vec3 H1 = normalize( L1 + E );
    vec3 H2 = normalize( L1 + E );
    vec3 H3 = normalize( L1 + E );
    
    vec3 ambient = ambient1 + ambient2 + ambient3;

    float Kd1 = max(dot(L1, N), 0.0);
    float Kd2 = max(dot(L2, N), 0.0);
    float Kd3 = max(dot(L3, N), 0.0);
    vec3 diffuse = Kd1*diffuse1 + Kd2*diffuse2 + Kd3*diffuse3;
    
    float Ks1 = pow(max(dot(N, H1), 0.0), shininess);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    // discard the specular highlight if the light's behind the vertex
    if( dot(L1, N) >= 0.0 ) {
        float Ks1 = pow(max(dot(N, H1), 0.0), shininess);
	specular += Ks1*specular1;
    }
    if( dot(L2, N) >= 0.0 ) {
        float Ks2 = pow(max(dot(N, H2), 0.0), shininess);
	specular += Ks2*specular2;
    }
    if( dot(L3, N) >= 0.0 ) {
        float Ks3 = pow(max(dot(N, H3), 0.0), shininess);
	specular += Ks3*specular3;
    }

    outColor = vec4(ambient + diffuse + specular, 1.0f);
}
