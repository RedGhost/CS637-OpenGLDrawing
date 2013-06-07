#include "BezierPatch.h"
#include "Angel.h"

#include <iostream>

using namespace std;

GLuint BezierPatch::program = 0;
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
GLint BezierPatch::uTexture = 0;

BezierPatch :: BezierPatch() {
    _currentPoint = 0;
    _selectedPoint = 0;
    _resolution = 10;
    _vertices = (vec3 *)malloc(sizeof(vec3));
    _normals = (vec3 *) malloc(sizeof(vec3));
    _texCoords = (vec2 *) malloc(sizeof(vec2));
    _faces = (GLuint *) malloc(sizeof(GLuint));
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
  glBufferData( GL_ARRAY_BUFFER, 44 * sizeof(vec3), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, 16 * sizeof(vec3), _controlPoints);

  vec3 axisPoints[6];
  axisPoints[0] = vec3(-10.0f, 0.0f, 0.0f);
  axisPoints[1] = vec3(10.0f, 0.0f, 0.0f);
  axisPoints[2] = vec3(0.0f, -10.0f, 0.0f);
  axisPoints[3] = vec3(0.0f, 10.0f, 0.0f);
  axisPoints[4] = vec3(0.0f, 0.0f, -10.0f);
  axisPoints[5] = vec3(0.0f, 0.0f, 10.0f);

  glBufferSubData( GL_ARRAY_BUFFER, 16 * sizeof(vec3), 6 * sizeof(vec3), axisPoints);

  vec3 colors[16];
  for(int i = 0; i < 16; i++) {
    if(i == _selectedPoint) {
      colors[i] = vec3(1.0f, 0.0f, 0.0f);
    }
    else {
      colors[i] = vec3(1.0f, 1.0f, 1.0f);
    }
  }

  vec3 axisColors[6];
  for(int i = 0; i < 6; i++) {
      axisColors[i] = vec3(1.0f, 1.0f, 1.0f);
  }

  glBufferSubData( GL_ARRAY_BUFFER, 22 * sizeof(vec3), 16 * sizeof(vec3), colors);
  glBufferSubData( GL_ARRAY_BUFFER, 38 * sizeof(vec3), 6 * sizeof(vec3), axisColors);

  GLuint point = glGetAttribLocation( controlPointProgram, "vPoint" );
  glVertexAttribPointer( point, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( point );

  GLuint color = glGetAttribLocation( controlPointProgram, "vColor" );
  glVertexAttribPointer( color, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(22 * sizeof(vec3)) );
  glEnableVertexAttribArray( color );

  glGenVertexArrays( 1, &vaoVertices );
  glBindVertexArray( vaoVertices );

  glGenBuffers( 1, &vboVertices );
  glBindBuffer( GL_ARRAY_BUFFER, vboVertices );

  glGenBuffers( 1, &eboVertices );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eboVertices );

  GLuint bezierPoint = glGetAttribLocation( program, "vPoint" );
  glVertexAttribPointer( bezierPoint, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( bezierPoint );

  GLuint bezierNormal = glGetAttribLocation( program, "vNormal" );
  glVertexAttribPointer( bezierNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((_resolution+1) * (_resolution+1)) * sizeof(vec3)) );
  glEnableVertexAttribArray( bezierNormal );

  GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
  glEnableVertexAttribArray( vTexCoord );
  glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((2 * (_resolution+1) * (_resolution+1)) * sizeof(vec3)) );

  recalculateVertices();

  _initialized = true;
}

void BezierPatch :: draw(mat4 modelView, mat4 projection, vec4 eyePosition, vec4 lightPosition, vec3 diffuse, vec3 ambient, vec3 specular) {
  if(_initialized) {
    //draw control points
    glUseProgram( controlPointProgram );
    glUniformMatrix4fv(uModelViewControlPoint, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(uProjectionControlPoint, 1, GL_TRUE, projection);

    glBindVertexArray( vaoControlPoints );
    glBindBuffer( GL_ARRAY_BUFFER, vboControlPoints );

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

    glBufferSubData( GL_ARRAY_BUFFER, 22 * sizeof(vec3), 16 * sizeof(vec3), colors);
    glDrawArrays( GL_POINTS, 0, 16 ); 
    glDrawArrays( GL_LINES, 16, 6 ); 

    //draw vertices
    glUseProgram( program ); 
    glUniformMatrix4fv(uModelView, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(uProjection, 1, GL_TRUE, projection);
    glUniform4fv(uLightPosition, 1, lightPosition);
    glUniform3fv(uDiffuse, 1, diffuse * _diffuse );
    glUniform3fv(uAmbient, 1, ambient * _ambient );
    glUniform3fv(uSpecular, 1, specular * _specular );
    glUniform4fv(uEyePosition, 1, eyePosition);
    glUniform1f(uShininess, _shininess);
    glUniform1i( uTexture, _textureID );

    glBindVertexArray( vaoVertices );

    glBindBuffer( GL_ARRAY_BUFFER, vboVertices );
    glBufferData( GL_ARRAY_BUFFER, ((2 * (_resolution+1) * (_resolution+1)) * sizeof(vec3)) + ((_resolution+1) * (_resolution+1) * sizeof(vec2)) , NULL, GL_STATIC_DRAW);
    glBufferSubData( GL_ARRAY_BUFFER, 0, ((_resolution+1) * (_resolution+1)) * sizeof(vec3), _vertices);
    glBufferSubData( GL_ARRAY_BUFFER, ((_resolution+1) * (_resolution+1)) * sizeof(vec3), ((_resolution+1) * (_resolution+1)) * sizeof(vec3), _normals);
    glBufferSubData( GL_ARRAY_BUFFER, 2 * ((_resolution+1) * (_resolution+1)) * sizeof(vec3), ((_resolution+1) * (_resolution+1)) * sizeof(vec2), _texCoords);

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eboVertices );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6*_resolution*_resolution*sizeof(GLuint), _faces, GL_STATIC_DRAW); 

    glDrawElements( GL_TRIANGLES, 6*_resolution*_resolution, GL_UNSIGNED_INT, 0 );
  }
}

void BezierPatch :: applyMaterial(vec3 diffuse, vec3 ambient, vec3 specular, GLfloat shininess) {
    _diffuse = diffuse;
    _ambient = ambient;
    _specular = specular;
    _shininess = shininess;
}

void BezierPatch :: applyTexture(GLuint textureID) {
    _textureID = textureID;
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
    recalculateVertices();
}

void BezierPatch::changeResolution(GLint resolution) {
    _resolution = resolution;

    glBindVertexArray( vaoVertices );
    GLuint point = glGetAttribLocation( program, "vPoint" );
    glVertexAttribPointer( point, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( point );

    GLuint bezierNormal = glGetAttribLocation( program, "vNormal" );
    glVertexAttribPointer( bezierNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((_resolution+1) * (_resolution+1)) * sizeof(vec3)) );
    glEnableVertexAttribArray( bezierNormal );

    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((2 * (_resolution+1) * (_resolution+1)) * sizeof(vec3)) );

    recalculateVertices();
}

void BezierPatch::changeResolutionBy(GLint resolution) {
    GLint res = _resolution + resolution;
    if(res < 1) {
        res = 1;
    }
    else if(res > 20) {
        res = 20;
    }
    changeResolution(res);
}

void BezierPatch::recalculateVertices() {
    free(_vertices);
    free(_normals);
    free(_texCoords);
    free(_faces);

    _vertices = (vec3*)malloc((_resolution+1) * (_resolution+1) * sizeof(vec3));
    _normals = (vec3*)malloc((_resolution+1) * (_resolution+1) * sizeof(vec3));
    _texCoords = (vec2*)malloc((_resolution+1) * (_resolution+1) * sizeof(vec2));
    _faces = (GLuint*)malloc((_resolution) * (_resolution) * 6 * sizeof(GLuint));
 
    _normalGeneration.clear();
    for(int i = 0; i < (_resolution+1) * (_resolution+1); i++) {
        _normalGeneration.push_back(vector<vec3>());        
    }

    for(int u = 0; u <= _resolution; u++) {
        for(int v = 0; v <= _resolution; v++) {
            int index = (_resolution+1) * u + v;
            _vertices[index] = vec3(0.0f, 0.0f, 0.0f);
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    _vertices[index] += (b(i, ((float)u / (float)_resolution)) * b(j, ((float)v / (float)_resolution)) * _controlPoints[4*i + j]);
                }
            }
            _normals[index] = _vertices[index];
            _texCoords[index] = vec2((float)u / (float)_resolution, (float)v / (float)_resolution);
        }
    }

    for(int u = 0; u < _resolution; u++) {
        for(int v = 0; v < _resolution; v++) {
            int faceIndex = 3*((2 *_resolution) * u + 2 * v);
            _faces[faceIndex] = ((_resolution+1)*u)+v;
            _faces[faceIndex+1] = ((_resolution+1)*(u+1))+v;
            _faces[faceIndex+2] = ((_resolution+1)*(u+1)+v+1);
            vec3 n = normalize(cross(_vertices[_faces[faceIndex+1]] - _vertices[_faces[faceIndex]], _vertices[_faces[faceIndex+2]] - _vertices[_faces[faceIndex+1]]));

            _normalGeneration[_faces[faceIndex]].push_back(n);
            _normalGeneration[_faces[faceIndex+1]].push_back(n);
            _normalGeneration[_faces[faceIndex+2]].push_back(n);

            faceIndex = 3 * ((2*_resolution) * u + 2 * v + 1);
            _faces[faceIndex] = ((_resolution+1)*u)+v;
            _faces[faceIndex+1] = ((_resolution+1)*(u+1))+v+1;
            _faces[faceIndex+2] = ((_resolution+1)*u)+v+1;
            n = normalize(cross(_vertices[_faces[faceIndex+1]] - _vertices[_faces[faceIndex]], _vertices[_faces[faceIndex+2]] - _vertices[_faces[faceIndex+1]]));

            _normalGeneration[_faces[faceIndex]].push_back(n);
            _normalGeneration[_faces[faceIndex+1]].push_back(n);
            _normalGeneration[_faces[faceIndex+2]].push_back(n);
        }
    }

    int i = 0;
    for(vector< vector< vec3 > >::iterator itr1 = _normalGeneration.begin(); itr1 != _normalGeneration.end(); itr1++) {
        vec3 normal = vec3(0.0f, 0.0f, 0.0f);
        for(vector<vec3>::iterator itr2 = itr1->begin(); itr2 != itr1->end(); itr2++) {
            normal += *(itr2);
        }
        _normals[i] = normal / itr1->size();
        i++;
    }
}

float BezierPatch::b(int index, float u) {
    switch(index) {
	case 0:
            return (1-u) * (1-u) * (1-u);
        case 1:
            return (3*u) * (1-u) * (1-u);
	case 2:
            return (3*(u*u)) * (1-u);
	case 3:
            return u*u*u;
    }
    return 0;
}

void BezierPatch::Initialize() {
  program = InitShader( "vBezierPatchShaderPhong.glsl", "fBezierPatchShaderPhong.glsl" );
  glBindFragDataLocation( program, 0, "outColor" );
  LinkProgram(program);

  controlPointProgram = InitShader( "vBezierPatchControlPointShader.glsl", "fBezierPatchControlPointShader.glsl" );
  glBindFragDataLocation( controlPointProgram, 0, "outColor" );
  LinkProgram(controlPointProgram);

  uModelViewControlPoint = glGetUniformLocation( controlPointProgram, "modelView" );
  uProjectionControlPoint = glGetUniformLocation( controlPointProgram, "projection" );

  uModelView = glGetUniformLocation( program, "modelView" );
  uProjection = glGetUniformLocation( program, "projection" );;
  uLightPosition = glGetUniformLocation( program, "lightPosition" );
  uDiffuse = glGetUniformLocation( program, "diffuse" );
  uAmbient = glGetUniformLocation( program, "ambient" );
  uSpecular = glGetUniformLocation( program, "specular" );
  uShininess = glGetUniformLocation( program, "shininess" );
  uEyePosition = glGetUniformLocation( program, "eyePosition" );
  uTexture = glGetUniformLocation( program, "myTexture" );
}

void BezierPatch::Deinitialize() {
  glDeleteProgram( program );
  glDeleteProgram( controlPointProgram );
}
