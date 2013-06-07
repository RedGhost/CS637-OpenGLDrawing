#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <list>

#include "Angel.h"

typedef struct Face {
    int one;
    int two;
    int three;
} Face;

class Model {
public:
    static GLuint phongProgram;
    static GLuint gouraudProgram;
    static GLuint activeProgram;
    static GLint uModelView;
    static GLint uProjection;
    static GLint uDiffuse1;
    static GLint uAmbient1;
    static GLint uSpecular1;
    static GLint uDiffuse2;
    static GLint uAmbient2;
    static GLint uSpecular2;
    static GLint uDiffuse3;
    static GLint uAmbient3;
    static GLint uSpecular3;
    static GLint uShininess;
    static GLint uLightPosition1;
    static GLint uLightPosition2;
    static GLint uLightPosition3;
    static GLint uEyePosition;
    static GLint uTexture;

    bool _initialized;
    GLuint vbo, vao, ebo;
    vec3 _diffuse, _ambient, _specular;
    GLfloat _shininess;
    GLuint _textureID;
    vec3 _minPoint;
    vec3 _maxPoint;

    void addVertex(vec3 point);
    void addFace(GLuint p1, GLuint p2, GLuint p3);

    Model();
    virtual void initialize();
    virtual void draw(mat4 modelView, mat4 projection, vec4 eyePosition, vec4 lightPosition1, vec3 diffuse1, vec3 ambient1, vec3 specular1, vec4 lightPosition2, vec3 diffuse2, vec3 ambient2, vec3 specular2, vec4 lightPosition3, vec3 diffuse3, vec3 ambient3, vec3 specular3) ;
    virtual void applyMaterial(vec3 diffuse, vec3 ambient, vec3 specular, GLfloat shininess);
    virtual void applyTexture(GLuint textureID);
    virtual void changeShader(int shader);

    static void Initialize();
    static void Deinitialize();

private:
    std::vector< std::vector< vec3 > > _normals;
    std::vector<vec3> _vertices;
    std::vector<vec3> _texCoords;
    std::vector<GLuint> _faces;
};

#endif
