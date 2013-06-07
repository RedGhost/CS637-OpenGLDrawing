#version 430

uniform mat4 modelView;
uniform mat4 projection;
uniform vec4 lightPosition1;
uniform vec4 lightPosition2;
uniform vec4 lightPosition3;
uniform vec4 eyePosition;
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

in vec3 vPoint;
in vec3 vNormal;
in vec3 vTexCoords;

out vec4 color;
out vec3 texCoord;

void
main()
{
    vec3 pos = (modelView * vec4(vPoint, 1.0)).xyz;
	
    vec3 L1 = normalize( (modelView*lightPosition1).xyz - pos );
    vec3 L2 = normalize( (modelView*lightPosition2).xyz - pos );
    vec3 L3 = normalize( (modelView*lightPosition3).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H1 = normalize( L1 + E );
    vec3 H2 = normalize( L2 + E );
    vec3 H3 = normalize( L3 + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( modelView*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = vec4(ambient1 + ambient2 + ambient3, 1.0f);

    float Kd1 = max( dot(L1, N), 0.0 );
    float Kd2 = max( dot(L2, N), 0.0 );
    float Kd3 = max( dot(L3, N), 0.0 );
    vec4  diffuse = vec4(Kd1*diffuse1 + Kd2*diffuse2 + Kd3*diffuse3, 1.0f);

    float Ks1 = pow( max(dot(N, H1), 0.0), shininess );
    float Ks2 = pow( max(dot(N, H2), 0.0), shininess );
    float Ks3 = pow( max(dot(N, H3), 0.0), shininess );
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
    
    if( dot(L1, N) >= 0.0 ) {
        specular += vec4(Ks1*specular1, 1.0f);
    } 
    if( dot(L2, N) >= 0.0 ) {
        specular += vec4(Ks2*specular2, 1.0f);
    } 
    if( dot(L3, N) >= 0.0 ) {
        specular += vec4(Ks3*specular3, 1.0f);
    } 

    texCoord = vTexCoords;

    gl_Position = projection * modelView * vec4(vPoint, 1.0f);

    color = ambient + diffuse + specular;
    color.a = 1.0;
}
