#version 430

in vec3 fN;
in vec3 fL;
in vec3 fE;

in  vec2 texCoord;

out vec4 outColor;

uniform vec3 diffuse;
uniform vec3 ambient;
uniform vec3 specular;
uniform float shininess;

uniform sampler2D myTexture;

void
main()
{
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);

    vec3 H = normalize( L + E );
    
    float Kd = max(dot(L, N), 0.0);
    vec3 diffuseColor = Kd*diffuse;

    float Ks = pow(max(dot(N, H), 0.0), shininess);
    vec3 specularColor = Ks*specular;
    // discard the specular highlight if the light's behind the vertex
    if( dot(L, N) < 0.0 ) {
        specularColor = vec3(0.0, 0.0, 0.0);
    }

    outColor = vec4(ambient + diffuseColor + specularColor, 1.0f) * texture( myTexture, texCoord );
}
