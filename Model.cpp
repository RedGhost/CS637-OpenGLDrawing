#include "Model.h"
#include "Angel.h"

#include <iostream>

using namespace std;

GLuint Model::activeProgram = 0;
GLuint Model::phongProgram = 0;
GLuint Model::gouraudProgram = 0;
GLint Model::uModelView = 0;
GLint Model::uProjection = 0;
GLint Model::uLightPosition1 = 0;
GLint Model::uLightPosition2 = 0;
GLint Model::uLightPosition3 = 0;
GLint Model::uDiffuse1 = 0;
GLint Model::uAmbient1 = 0;
GLint Model::uSpecular1 = 0;
GLint Model::uDiffuse2 = 0;
GLint Model::uAmbient2 = 0;
GLint Model::uSpecular2 = 0;
GLint Model::uDiffuse3 = 0;
GLint Model::uAmbient3 = 0;
GLint Model::uSpecular3 = 0;
GLint Model::uShininess = 0;
GLint Model::uEyePosition = 0;
GLint Model::uTexture = 0;

Model :: Model() {
    _minPoint = vec3(1000.0f, 1000.0f, 1000.0f);
    _maxPoint = vec3(-1000.0f, -1000.0f, -1000.0f);
}

void Model :: addVertex(vec3 point) {
    _vertices.push_back(point); 
    if(_minPoint.x > point.x) {
      _minPoint.x = point.x;
    }
    if(_minPoint.y > point.y) {
      _minPoint.y = point.y;
    }
    if(_minPoint.z > point.z) {
      _minPoint.z = point.z;
    }
    if(_maxPoint.x < point.x) {
      _maxPoint.x = point.x;
    }
    if(_maxPoint.y < point.y) {
      _maxPoint.y = point.y;
    }
    if(_maxPoint.z < point.z) {
      _maxPoint.z = point.z;
    }
    _normals.push_back(vector<vec3>());
}

void Model :: addFace(GLuint p1, GLuint p2, GLuint p3) {
    _faces.push_back(p1-1);
    _faces.push_back(p2-1);
    _faces.push_back(p3-1);

    vec3 n = normalize(cross(_vertices[p2-1] - _vertices[p1-1], _vertices[p3-1] - _vertices[p2-1]));

    _normals[p1-1].push_back(n);
    _normals[p2-1].push_back(n);
    _normals[p3-1].push_back(n);
}

void Model::initialize() {
  glGenVertexArrays( 1, &vao);
  glBindVertexArray( vao );

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

  vec3 offset = -_minPoint;
  vec3 divisor = offset + _maxPoint;

  for(vector< vec3 >::iterator itr = _vertices.begin(); itr != _vertices.end(); itr++) {
    vec3 texCoord = vec3(((*itr).x + offset.x) / divisor.x, ((*itr).y + offset.y) / divisor.y, ((*itr).z + offset.z) / divisor.z);
    _texCoords.push_back(texCoord);
  }
  vec3 * texCoords = &(_texCoords[0]);

  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glBufferData( GL_ARRAY_BUFFER, (_vertices.size() + _normals.size() + _texCoords.size()) * sizeof(vec3), NULL, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(vec3), vertices);
  glBufferSubData( GL_ARRAY_BUFFER, _vertices.size() * sizeof(vec3), _normals.size() * sizeof(vec3), normals);
  glBufferSubData( GL_ARRAY_BUFFER, (_vertices.size() + _normals.size()) * sizeof(vec3), _texCoords.size() * sizeof(vec3), texCoords);

  free(normals);

  glGenBuffers( 1, &ebo );
  GLuint * faces = &(_faces[0]);
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, _faces.size()  * sizeof(GLuint), faces, GL_STATIC_DRAW );

  GLuint point = glGetAttribLocation( activeProgram, "vPoint" );
  glVertexAttribPointer( point, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( point );

  GLuint normal = glGetAttribLocation( activeProgram, "vNormal" );
  glVertexAttribPointer( normal, 3, GL_FLOAT, GL_FALSE, 0, 
                         BUFFER_OFFSET(_vertices.size() * sizeof(vec3)) );
  glEnableVertexAttribArray( normal );

  GLuint vTexCoords = glGetAttribLocation( activeProgram, "vTexCoords" );
  glVertexAttribPointer( vTexCoords, 3, GL_FLOAT, GL_FALSE, 0, 
                         BUFFER_OFFSET(2 * _vertices.size() * sizeof(vec3)) );
  glEnableVertexAttribArray( vTexCoords );

  _initialized = true;
}

void Model :: draw(mat4 modelView, mat4 projection, vec4 eyePosition, vec4 lightPosition1, vec3 diffuse1, vec3 ambient1, vec3 specular1, vec4 lightPosition2, vec3 diffuse2, vec3 ambient2, vec3 specular2, vec4 lightPosition3, vec3 diffuse3, vec3 ambient3, vec3 specular3) {
  if(_initialized) {
    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );

    glUseProgram( activeProgram );

    glUniformMatrix4fv(uModelView, 1, GL_TRUE, modelView);
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
    glUniform1f(uTexture, _textureID);

    glDrawElements( GL_TRIANGLES, _faces.size(), GL_UNSIGNED_INT, 0 );
  }
}

void Model :: applyMaterial(vec3 diffuse, vec3 ambient, vec3 specular, GLfloat shininess) {
    _diffuse = diffuse;
    _ambient = ambient;
    _specular = specular;
    _shininess = shininess;
}

void Model :: applyTexture(GLuint textureID) {
    _textureID = textureID;
}

void Model :: changeShader(int shader) {
    if(shader == 0) {
      activeProgram = phongProgram;
    }
    else {
      activeProgram = gouraudProgram;
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

    GLuint texCoords = glGetAttribLocation( activeProgram, "vTexCoords" );
    glVertexAttribPointer( texCoords, 3, GL_FLOAT, GL_FALSE, 0, 
                         BUFFER_OFFSET(2 * _vertices.size() * sizeof(vec3)) );
    glEnableVertexAttribArray( texCoords );

    uModelView = glGetUniformLocation( activeProgram, "modelView" );
    uProjection = glGetUniformLocation( activeProgram, "projection" );;
    uLightPosition1 = glGetUniformLocation( activeProgram, "lightPosition1" );
    uLightPosition2 = glGetUniformLocation( activeProgram, "lightPosition2" );
    uLightPosition3 = glGetUniformLocation( activeProgram, "lightPosition3" );
    uDiffuse1 = glGetUniformLocation( activeProgram, "diffuse1" );
    uAmbient1 = glGetUniformLocation( activeProgram, "ambient1" );
    uSpecular1 = glGetUniformLocation( activeProgram, "specular1" );
    uDiffuse2 = glGetUniformLocation( activeProgram, "diffuse2" );
    uAmbient2 = glGetUniformLocation( activeProgram, "ambient2" );
    uSpecular2 = glGetUniformLocation( activeProgram, "specular2" );
    uDiffuse3 = glGetUniformLocation( activeProgram, "diffuse3" );
    uAmbient3 = glGetUniformLocation( activeProgram, "ambient3" );
    uSpecular3 = glGetUniformLocation( activeProgram, "specular3" );
    uShininess = glGetUniformLocation( activeProgram, "shininess" );
    uEyePosition = glGetUniformLocation( activeProgram, "eyePosition" );
    uTexture = glGetUniformLocation( activeProgram, "myTexture" );
}

void Model::Initialize() {
  phongProgram = InitShader( "vModelShaderPhong.glsl", "fModelShaderPhong.glsl" );
  glBindFragDataLocation( phongProgram, 0, "outColor" );
  LinkProgram(phongProgram);

  gouraudProgram = InitShader( "vModelShaderGouraud.glsl", "fModelShaderGouraud.glsl" );
  glBindFragDataLocation( gouraudProgram, 0, "outColor" );
  LinkProgram(gouraudProgram);

  activeProgram = phongProgram;

  uModelView = glGetUniformLocation( activeProgram, "modelView" );
  uProjection = glGetUniformLocation( activeProgram, "projection" );;
  uLightPosition1 = glGetUniformLocation( activeProgram, "lightPosition1" );
  uLightPosition2 = glGetUniformLocation( activeProgram, "lightPosition2" );
  uLightPosition3 = glGetUniformLocation( activeProgram, "lightPosition3" );
  uDiffuse1 = glGetUniformLocation( activeProgram, "diffuse1" );
  uAmbient1 = glGetUniformLocation( activeProgram, "ambient1" );
  uSpecular1 = glGetUniformLocation( activeProgram, "specular1" );
  uDiffuse2 = glGetUniformLocation( activeProgram, "diffuse2" );
  uAmbient2 = glGetUniformLocation( activeProgram, "ambient2" );
  uSpecular2 = glGetUniformLocation( activeProgram, "specular2" );
  uDiffuse3 = glGetUniformLocation( activeProgram, "diffuse3" );
  uAmbient3 = glGetUniformLocation( activeProgram, "ambient3" );
  uSpecular3 = glGetUniformLocation( activeProgram, "specular3" );
  uShininess = glGetUniformLocation( activeProgram, "shininess" );
  uEyePosition = glGetUniformLocation( activeProgram, "eyePosition" );
  uTexture = glGetUniformLocation( activeProgram, "myTexture" );
}

void Model::Deinitialize() {
  glDeleteProgram( phongProgram );
  glDeleteProgram( gouraudProgram );
}
