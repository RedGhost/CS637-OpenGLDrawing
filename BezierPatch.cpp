#include "BezierPatch.h"
#include "Angel.h"

#include <iostream>

using namespace std;

GLuint BezierPatch::activeProgram = 0;
GLuint BezierPatch::phongProgram = 0;
GLuint BezierPatch::flatProgram = 0;
GLuint BezierPatch::controlPointProgram = 0;
GLint BezierPatch::uBezierPatchView = 0;
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
}

void BezierPatch :: addControlPoint(vec3 point) {
    _controlPoints[_currentPoint] = point;
    _currentPoint++;
}

void BezierPatch::initialize() {
  glGenVertexArrays( 1, &vao);
  glBindVertexArray( vao );

  glGenBuffers(1, &vbo);
  glBindBuffer( GL_ARRAY_BUFFER, vbo);
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

/*
  for(int i = 0; i< 16; i++) {
    cout << _controlPoints[i] << endl;
  }

  glGenBuffers( 1, &vbo);  
  vec3 * vertices = &(_vertices[0]);
  vec3 * normals = (vec3 *) malloc(_normals.size() * sizeof(vec3));
  int i = 0;
  for(vector< vector< vec3 > >::iterator itr1 = _normals.begin(); itr1 != _normals.end(); itr1++) {
    vec3 normal = vec3(0.0f, 0.0f, 0.0f);
    for(vector<vec3>::iterator itr2 = itr1->begin(); itr2 != itr1->end(); itr2++) {
      normal += *(itr2);
    }
    normals[i] = normal / itr1->size();
    i++;
  }

  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glBufferData( GL_ARRAY_BUFFER, (_vertices.size() + _normals.size()) * sizeof(vec3), NULL, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(vec3), vertices);
  glBufferSubData( GL_ARRAY_BUFFER, _vertices.size() * sizeof(vec3), _normals.size() * sizeof(vec3), normals);

  free(normals);

  glGenBuffers( 1, &ebo );
  GLuint * faces = &(_faces[0]);
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, _faces.size()  * sizeof(GLuint), faces, GL_STATIC_DRAW );
*/
  GLuint point = glGetAttribLocation( controlPointProgram, "vPoint" );
  glVertexAttribPointer( point, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( point );

  GLuint color = glGetAttribLocation( controlPointProgram, "vColor" );
  glVertexAttribPointer( color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(16 * sizeof(vec3)) );
  glEnableVertexAttribArray( color );

  _initialized = true;
}

void BezierPatch :: draw(mat4 modelView, mat4 projection, vec4 eyePosition, vec4 lightPosition, vec3 diffuse, vec3 ambient, vec3 specular) {
  if(_initialized) {
    glUseProgram( controlPointProgram );
    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    glUniformMatrix4fv(uBezierPatchView, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(uProjection, 1, GL_TRUE, projection);

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
  /*  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );

    glUseProgram( activeProgram );

    glUniformMatrix4fv(uBezierPatchView, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(uProjection, 1, GL_TRUE, projection);
    glUniform4fv(uLightPosition1, 1, lightPosition1);
    glUniform3fv(uDiffuse1, 1, diffuse1 * _diffuse );
    glUniform3fv(uAmbient1, 1, ambient1 * _ambient );
    glUniform3fv(uSpecular1, 1, specular1 * _specular );
    glUniform4fv(uLightPosition2, 1, lightPosition2);
    glUniform3fv(uDiffuse2, 1, diffuse2 * _diffuse );
    glUniform3fv(uAmbient2, 1, ambient2 * _ambient );
    glUniform3fv(uSpecular2, 1, specular2 * _specular );
    glUniform4fv(uLightPosition3, 1, lightPosition3);
    glUniform3fv(uDiffuse3, 1, diffuse3 * _diffuse );
    glUniform3fv(uAmbient3, 1, ambient3 * _ambient );
    glUniform3fv(uSpecular3, 1, specular3 * _specular );
    glUniform4fv(uEyePosition, 1, eyePosition);
    glUniform1f(uShininess, _shininess);

    glDrawElements( GL_TRIANGLES, _faces.size(), GL_UNSIGNED_INT, 0 );
 */ }
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

    glBindVertexArray( vao );

    GLuint point = glGetAttribLocation( activeProgram, "vPoint" );
    glVertexAttribPointer( point, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( point );

    GLuint normal = glGetAttribLocation( activeProgram, "vNormal" );
    glVertexAttribPointer( normal, 3, GL_FLOAT, GL_FALSE, 0, 
                         BUFFER_OFFSET(_vertices.size() * sizeof(vec3)) );
    glEnableVertexAttribArray( normal );

    uBezierPatchView = glGetUniformLocation( controlPointProgram, "modelView" );
    uProjection = glGetUniformLocation( controlPointProgram, "projection" );;
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

  uBezierPatchView = glGetUniformLocation( controlPointProgram, "modelView" );
  uProjection = glGetUniformLocation( controlPointProgram, "projection" );;
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
