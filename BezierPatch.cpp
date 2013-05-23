#include "BezierPatch.h"
#include "Angel.h"

#include <iostream>

using namespace std;

GLuint BezierPatch::activeProgram = 0;
GLuint BezierPatch::phongProgram = 0;
GLuint BezierPatch::flatProgram = 0;
GLuint BezierPatch::controlPointProgram = 0;
GLint BezierPatch::uModelViewControlPoint = 0;
GLint BezierPatch::uProjectionControlPoint = 0;
GLint BezierPatch::uModelView = 0;
GLint BezierPatch::uProjection = 0;
GLint BezierPatch::uLightPosition = 0;
GLint BezierPatch::uDiffuse = 0;
GLint BezierPatch::uAmbient = 0;
GLint BezierPatch::uSpecular = 0;
GLint BezierPatch::uShininess = 0;
GLint BezierPatch::uEyePosition = 0;

BezierPatch :: BezierPatch() {
    _currentPoint = 0;
    _selectedPoint = 0;
    _resolution = 10;
}

void BezierPatch :: addControlPoint(vec3 point) {
    _controlPoints[_currentPoint] = point;
    _currentPoint++;
}

void BezierPatch::initialize() {
  glGenVertexArrays( 1, &vaoControlPoints);
  glBindVertexArray( vaoControlPoints );

  glGenBuffers( 1, &vboControlPoints );
  glBindBuffer( GL_ARRAY_BUFFER, vboControlPoints );
  glBufferData( GL_ARRAY_BUFFER, 32 * sizeof(vec3), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, 16 * sizeof(vec3), _controlPoints);

  vec3 colors[16];
  for(int i = 0; i < 16; i++) {
    if(i == _selectedPoint) {
      colors[i] = vec3(1.0f, 0.0f, 0.0f);
    }
    else {
      colors[i] = vec3(1.0f, 1.0f, 1.0f);
    }
  }

  glBufferSubData( GL_ARRAY_BUFFER, 16 * sizeof(vec3), 16 * sizeof(vec3), colors);

  GLuint point = glGetAttribLocation( controlPointProgram, "vPoint" );
  glVertexAttribPointer( point, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( point );

  GLuint color = glGetAttribLocation( controlPointProgram, "vColor" );
  glVertexAttribPointer( color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(16 * sizeof(vec3)) );
  glEnableVertexAttribArray( color );

  glGenVertexArrays( 1, &vaoVertices );
  glBindVertexArray( vaoVertices );

  glGenBuffers( 1, &vboVertices );
  glGenBuffers( 1, &eboVertices );
  glBindBuffer( GL_ARRAY_BUFFER, vboVertices );

  GLuint bezierPoint = glGetAttribLocation( activeProgram, "vPoint" );
  glVertexAttribPointer( bezierPoint, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( point );

  GLuint bezierNormal = glGetAttribLocation( activeProgram, "vNormal" );
  glVertexAttribPointer( bezierNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((_resolution * _resolution) * sizeof(vec3)) );
  glEnableVertexAttribArray( bezierNormal );

  _initialized = true;
}

void BezierPatch :: draw(mat4 modelView, mat4 projection, vec4 eyePosition, vec4 lightPosition, vec3 diffuse, vec3 ambient, vec3 specular) {
  if(_initialized) {
    glUseProgram( controlPointProgram );
    glBindVertexArray( vaoControlPoints );
    glBindBuffer( GL_ARRAY_BUFFER, vboControlPoints );

    glUniformMatrix4fv(uModelViewControlPoint, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(uProjectionControlPoint, 1, GL_TRUE, projection);

    glBufferSubData( GL_ARRAY_BUFFER, 0, 16 * sizeof(vec3), _controlPoints);

    vec3 colors[16];
    for(int i = 0; i < 16; i++) {
      if(i == _selectedPoint) {
        colors[i] = vec3(1.0f, 0.0f, 0.0f);
      }
      else {
        colors[i] = vec3(1.0f, 1.0f, 1.0f);
      }
    }

    glBufferSubData( GL_ARRAY_BUFFER, 16 * sizeof(vec3), 16 * sizeof(vec3), colors);
    glDrawArrays( GL_POINTS, 0, 16 ); 

    glUseProgram( activeProgram );
    glBindVertexArray( vaoVertices );
    glBindBuffer( GL_ARRAY_BUFFER, vboVertices );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eboVertices );

    //draw vertices
    glUniformMatrix4fv(uModelView, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(uProjection, 1, GL_TRUE, projection);
    glUniform4fv(uLightPosition, 1, lightPosition3);
    glUniform3fv(uDiffuse, 1, diffuse * _diffuse );
    glUniform3fv(uAmbient, 1, ambient * _ambient );
    glUniform3fv(uSpecular, 1, specular * _specular );
    glUniform4fv(uEyePosition, 1, eyePosition);
    glUniform1f(uShininess, _shininess);

    glBufferData( GL_ARRAY_BUFFER, (2 * _resolution * _resolution) * sizeof(vec3), NULL, GL_STATIC_DRAW);
    glBufferSubData( GL_ARRAY_BUFFER, 0, (_resolution * _resolution) * sizeof(vec3), _vertices);
    glBufferSubData( GL_ARRAY_BUFFER, (_resolution * _resolution) * sizeof(vec3), (_resolution * _resolution) * sizeof(vec3), _normals);

    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 

    glDrawArrays( GL_TRIANGLES, 0, 16 ); 
  }
}

void BezierPatch :: applyMaterial(vec3 diffuse, vec3 ambient, vec3 specular, GLfloat shininess) {
    _diffuse = diffuse;
    _ambient = ambient;
    _specular = specular;
    _shininess = shininess;
}

void BezierPatch :: changeShader(int shader) {
    if(shader == 0) {
      activeProgram = phongProgram;
    }
    else {
      activeProgram = flatProgram;
    }

    glUseProgram( activeProgram );

    glBindVertexArray( vaoVertices );

    GLuint point = glGetAttribLocation( activeProgram, "vPoint" );
    glVertexAttribPointer( point, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( point );

    GLuint normal = glGetAttribLocation( activeProgram, "vNormal" );
    glVertexAttribPointer( normal, 3, GL_FLOAT, GL_FALSE, 0, 
                         BUFFER_OFFSET(_resolution * _resolution) );
    glEnableVertexAttribArray( normal );

    uModelView = glGetUniformLocation( activeProgram, "modelView" );
    uProjection = glGetUniformLocation( activeProgram, "projection" );;
    uLightPosition = glGetUniformLocation( activeProgram, "lightPosition" );
    uDiffuse = glGetUniformLocation( activeProgram, "diffuse" );
    uAmbient = glGetUniformLocation( activeProgram, "ambient" );
    uSpecular = glGetUniformLocation( activeProgram, "specular" );
    uShininess = glGetUniformLocation( activeProgram, "shininess" );
    uEyePosition = glGetUniformLocation( activeProgram, "eyePosition" );
}

void BezierPatch::selectPointToLeft() {
    if(_selectedPoint == 0) {
      _selectedPoint = 3;
    }
    else if(_selectedPoint == 4) {
      _selectedPoint = 7;
    }
    else if(_selectedPoint == 8) {
      _selectedPoint = 11;
    }
    else if(_selectedPoint == 12) {
      _selectedPoint = 15;
    }
    else {
      _selectedPoint--;
    }
}

void BezierPatch::selectPointToRight() {
    if(_selectedPoint == 3) {
      _selectedPoint = 0;
    }
    else if(_selectedPoint == 7) {
      _selectedPoint = 4;
    }
    else if(_selectedPoint == 11) {
      _selectedPoint = 8;
    }
    else if(_selectedPoint == 15) {
      _selectedPoint = 12;
    }
    else {
      _selectedPoint++;
    }
}

void BezierPatch::selectPointToUp() {
    if(_selectedPoint == 12) {
      _selectedPoint = 0;
    }
    else if(_selectedPoint == 13) {
      _selectedPoint = 1;
    }
    else if(_selectedPoint == 14) {
      _selectedPoint = 2;
    }
    else if(_selectedPoint == 15) {
      _selectedPoint = 3;
    }
    else {
      _selectedPoint += 4;
    }
}

void BezierPatch::selectPointToDown() {
    if(_selectedPoint == 0) {
      _selectedPoint = 12;
    }
    else if(_selectedPoint == 1) {
      _selectedPoint = 13;
    }
    else if(_selectedPoint == 2) {
      _selectedPoint = 14;
    }
    else if(_selectedPoint == 3) {
      _selectedPoint = 15;
    }
    else {
      _selectedPoint -= 4;
    }
}

void BezierPatch::changePointBy(GLfloat dx, GLfloat dy, GLfloat dz) {
    _controlPoints[_selectedPoint] += vec3(dx, dy, dz);
}

void BezierPatch::changeResolution(GLint resolution) {
    _resolution = resolution;
    recalculateVertices();
}

void BezierPatch::recalculateVerticss() {
    free(_vertices);
    free(_normals);

    
}

void BezierPatch::Initialize() {
  phongProgram = InitShader( "vBezierPatchShaderPhong.glsl", "fBezierPatchShaderPhong.glsl" );
  glBindFragDataLocation( phongProgram, 0, "outColor" );
  LinkProgram(phongProgram);

  flatProgram = InitShader( "vBezierPatchShaderFlat.glsl", "fBezierPatchShaderFlat.glsl" );
  glBindFragDataLocation( flatProgram, 0, "outColor" );
  LinkProgram(flatProgram);

  controlPointProgram = InitShader( "vBezierPatchControlPointShader.glsl", "fBezierPatchControlPointShader.glsl" );
  glBindFragDataLocation( controlPointProgram, 0, "outColor" );
  LinkProgram(controlPointProgram);

  activeProgram = phongProgram;

  uModelViewControlPoint = glGetUniformLocation( controlPointProgram, "modelView" );
  uProjectionControlPoint = glGetUniformLocation( controlPointProgram, "projection" );;

  uModelView = glGetUniformLocation( activeProgram, "modelView" );
  uProjection = glGetUniformLocation( activeProgram, "projection" );;
  uLightPosition = glGetUniformLocation( activeProgram, "lightPosition" );
  uDiffuse = glGetUniformLocation( activeProgram, "diffuse" );
  uAmbient = glGetUniformLocation( activeProgram, "ambient" );
  uSpecular = glGetUniformLocation( activeProgram, "specular" );
  uShininess = glGetUniformLocation( activeProgram, "shininess" );
  uEyePosition = glGetUniformLocation( activeProgram, "eyePosition" );
}

void BezierPatch::Deinitialize() {
  glDeleteProgram( phongProgram );
  glDeleteProgram( flatProgram );
  glDeleteProgram( controlPointProgram );
}
