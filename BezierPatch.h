#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include <vector>
#include <list>

#include "Angel.h"

class BezierPatch {
public:
    static GLuint phongProgram;
    static GLuint flatProgram;
    static GLuint controlPointProgram;
    static GLuint activeProgram;
    static GLint uBezierPatchView;
    static GLint uProjection;
    static GLint uDiffuse;
    static GLint uAmbient;
    static GLint uSpecular;
    static GLint uShininess;
    static GLint uLightPosition;
    static GLint uEyePosition;

    BezierPatch();

    void addControlPoint(vec3 point);

    virtual void initialize();
    virtual void draw(mat4 modelView, mat4 projection, vec4 eyePosition, vec4 lightPosition, vec3 diffuse, vec3 ambient, vec3 specular) ;
    virtual void applyMaterial(vec3 diffuse, vec3 ambient, vec3 specular, GLfloat shininess);
    virtual void changeShader(int shader);

    virtual void selectPointToLeft();
    virtual void selectPointToRight();
    virtual void selectPointToUp();
    virtual void selectPointToDown();
    virtual void changePointBy(GLfloat dx, GLfloat dy, GLfloat dz);

    static void Initialize();
    static void Deinitialize();

private:
    GLint _currentPoint;
    std::vector< std::vector< vec3 > > _normals;
    vec3 _controlPoints[16];
    std::vector<vec3> _vertices;
    std::vector<GLuint> _faces;

    bool _initialized;
    GLuint vbo, vao, ebo;
    vec3 _diffuse, _ambient, _specular;
    GLint _selectedPoint;
    GLfloat _shininess;
};

#endif
