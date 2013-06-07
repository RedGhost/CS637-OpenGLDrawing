// First new GL program
// Just makes a red triangle

#include <cstdlib>
#include <list>
#include <ctime>

#include "Angel.h"
#include "Model.h"
#include "SMFParser.h"

using namespace std;

//--------------------------------------------------------------------------

int mainWindow;
int mode = 1;

GLfloat camera_radius = 1.0f;
GLfloat camera_height = 0.0f;
GLfloat camera_speed = 0.1f;
GLfloat camera_rotation = 0.0f;

vec4 camera_position = vec4(0.0f, 0.0f, 0.0f, 1.0);
vec4 camera_up = vec4(0.0, 1.0, 0.0, 0.0);
vec4 camera_look_at = vec4(0.0, 0.0, 0.0, 1.0);

vec4 light_position1 = vec4(0.0f, 2.0f, 0.0f, 1.0f);
vec3 light_diffuse1 = vec3(0.5f, 0.5f, 0.5f);
vec3 light_ambient1 = vec3(0.2f, 0.2f, 0.2f);
vec3 light_specular1 = vec3(0.5f, 0.5f, 0.5f);

vec4 light_position2 = vec4(0.0f, -2.0f, 0.0f, 1.0f);
vec3 light_diffuse2 = vec3(0.5f, 0.5f, 0.5f);
vec3 light_ambient2 = vec3(0.2f, 0.2f, 0.2f);
vec3 light_specular2 = vec3(0.5f, 0.5f, 0.5f);

vec4 light_position3 = vec4(0.0f, 0.0f, 2.0f, 1.0f);
vec3 light_diffuse3 = vec3(0.5f, 0.5f, 0.5f);
vec3 light_ambient3 = vec3(0.2f, 0.2f, 0.2f);
vec3 light_specular3 = vec3(0.5f, 0.5f, 0.5f);
GLfloat light_radius = -2.0f;
GLfloat light_height = 0.0f;
GLfloat light_rotation = 0.0f;
GLfloat light_speed = 0.1f;

vec3 pink_material_diffuse = vec3(1.0f, 1.0f, 1.0f);
vec3 pink_material_ambient = vec3(1.0f, 1.0f, 1.0f);
vec3 pink_material_specular = vec3(1.0f, 1.0f, 1.0f);
GLfloat pink_material_shininess = 5.0f;

vec3 blue_material_diffuse = vec3(0.0f, 0.8f, 0.0f);
vec3 blue_material_ambient = vec3(0.0f, 0.0f, 0.5f);
vec3 blue_material_specular = vec3(0.0f, 0.0f, 1.0f);
GLfloat blue_material_shininess = 50.0f;

vec3 stone_material_diffuse = vec3(0.2f, 0.2f, 0.2f);
vec3 stone_material_ambient = vec3(0.5f, 0.5f, 0.5f);
vec3 stone_material_specular = vec3(0.8f, 0.8f, 0.8f);
GLfloat stone_material_shininess = 10.0f;

const int TextureSize = 64;
GLubyte textureData[TextureSize][TextureSize][TextureSize][3];
GLuint texture;

list<Model*> models;

#define noiseWidth 64
#define noiseHeight 64
#define noiseDepth 64


// Noise generation function taken from the internet
double noise[noiseWidth][noiseHeight][noiseDepth]; //the noise array
void generateNoise()
{
  for (int x = 0; x < noiseWidth; x++) {
    for (int y = 0; y < noiseHeight; y++) {
      for (int z = 0; z < noiseDepth; z++) {
        noise[x][y][z] = (rand() % 32768) / 32768.0;
      }
    }
  }
}


// Smoothing of noise function to get less pixely
double smoothNoise(double x, double y, double z)
{  
   //get fractional part of x and y
   double fractX = x - int(x);
   double fractY = y - int(y);
   double fractZ = z - int(z);
   
   //wrap around
   int x1 = (int(x) + noiseWidth) % noiseWidth;
   int y1 = (int(y) + noiseHeight) % noiseHeight;
   int z1 = (int(z) + noiseDepth) % noiseDepth;
   
   //neighbor values
   int x2 = (x1 + noiseWidth - 1) % noiseWidth;
   int y2 = (y1 + noiseHeight - 1) % noiseHeight;
   int z2 = (z1 + noiseDepth - 1) % noiseDepth;

   //smooth the noise with bilinear interpolation
   double value = 0.0;
   value += fractX       * fractY       * fractZ       * noise[x1][y1][z1];
   value += fractX       * (1 - fractY) * fractZ       * noise[x1][y2][z1];
   value += (1 - fractX) * fractY       * fractZ       * noise[x2][y1][z1];
   value += (1 - fractX) * (1 - fractY) * fractZ       * noise[x2][y2][z1];

   value += fractX       * fractY       * (1 - fractZ) * noise[x1][y1][z2];
   value += fractX       * (1 - fractY) * (1 - fractZ) * noise[x1][y2][z2];
   value += (1 - fractX) * fractY       * (1 - fractZ) * noise[x2][y1][z2];
   value += (1 - fractX) * (1 - fractY) * (1 - fractZ) * noise[x2][y2][z2];

   return value;
}

// Smooths the noise and returns a lightness attribute
double turbulence(double x, double y, double z, double size)
{
    double value = 0.0, initialSize = size;
    
    while(size >= 1)
    {
        value += smoothNoise(x / size, y / size, z / size) * size;
        size /= 2.0;
    }
    
    return(128.0 * value / initialSize);
}

// HSL to RGB conversion function taken from the internet
vec3 hslToRGB(vec3 hslColor) {
   float h = hslColor.x / 255.0f;
   float sl = hslColor.y / 255.0f;
   float l = hslColor.z / 255.0f;

   double v;
   double r,g,b;

   r = l;   // default to gray
   g = l;
   b = l;

   v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
   if (v > 0) {
    double m;
    double sv;
    int sextant;
    double fract, vsf, mid1, mid2;
 
    m = l + l - v;
    sv = (v - m ) / v;
    h *= 6.0;
    sextant = (int)h;
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = m + vsf;
    mid2 = v - vsf;
    switch (sextant) {
    case 0:
      r = v;
      g = mid1;
      b = m;
      break;
    case 1:
      r = mid2;
      g = v;
      b = m;
      break;
    case 2:
      r = m;
      g = v;
      b = mid1;
    case 3:
      r = m;
      g = mid2;
      b = v;
      break;
    case 4:
      r = mid1;
      g = m;
      b = v;
      break;
    case 5:
      r = v;
      g = m;
      b = mid2;
      break;
    }
  }
  return vec3(r * 255, g * 255, b * 255);;
}

void
init( void )
{
    srand (time(NULL));
    generateNoise();
    // Create a checkerboard pattern
    for ( int x = 0; x < 64; x++ ) {
        for ( int y = 0; y < 64; y++ ) {
            for ( int z = 0; z < 64; z++ ) {
                float lightness =  170.0f + (turbulence(x, y, z, 8) / 4.0f);
                vec3 color = hslToRGB(vec3(169, 255, lightness));       
        
                textureData[x][y][z][0]  = color.x;
                textureData[x][y][z][1]  = color.y;
                textureData[x][y][z][2]  = color.z;
            }
        }
    }
    // Initialize texture objects
    glGenTextures( 1, &texture );

    glBindTexture( GL_TEXTURE_3D, texture );
    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGB, TextureSize, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D, texture );

    char * inFile = (char*)"bound-bunny_5k.smf";
    SMFParser parser(inFile);

    models = parser.parse();
    list<Model*>::iterator iterator = models.begin();
    while(iterator != models.end()) {
        (*iterator)->initialize();
        (*iterator)->applyMaterial(pink_material_diffuse, pink_material_ambient, pink_material_specular, pink_material_shininess);
        (*iterator)->applyTexture(0);
        iterator++;
    }
}

void
dealloc( void ) {
  Model::Deinitialize();
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClearColor( 0.1, 0.1, 0.1, 1.0 ); // black background
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

    vec4 current_camera_position = RotateY(camera_rotation) * Translate(0.0f, camera_height, -camera_radius) * camera_position;
    mat4 modelView = LookAt(current_camera_position, camera_look_at, camera_up);
    mat4 projection;

    vec4 light3Position = RotateY(light_rotation) * Translate(0.0f, light_height, -light_radius) * light_position3;

    if(mode == 1) {
        projection = Ortho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 100.0f);
    }
    else {
        projection = Perspective(45.0f, 1.0f, 0.5f, 100.0f);
    }

    list<Model*>::iterator iterator = models.begin();

    while(iterator != models.end()) {
        (*iterator)->draw(modelView, projection, current_camera_position, light_position1, light_diffuse1, light_ambient1, light_specular1, light_position2, light_diffuse2, light_ambient2, light_specular2, light3Position, light_diffuse3, light_ambient3, light_specular3);
        iterator++;
    }

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    case 'q':
        camera_height += 0.1;
        break;
    case 'a':
        camera_height -= 0.1;
        break;
    case 'w':
        camera_radius -= 0.1;
        if(camera_radius < 0.0f) {
          camera_radius = 0.0f;
        }
        break;
    case 's':
        camera_radius += 0.1;
        break;
    case 'e':
        camera_speed += 0.1;
        break;
    case 'd':
        camera_speed -= 0.1;
        if(camera_speed < 0.0f) {
          camera_speed = 0.0f;
        }
        break;
    case 'r':
        light_height += 0.1;
        break;
    case 'f':
        light_height -= 0.1;
        break;
    case 't':
        light_radius -= 0.1;
        if(light_radius < 0.0f) {
          light_radius = 0.0f;
        }
        break;
    case 'g':
        light_radius += 0.1;
        break;
    case 'y':
        light_speed += 0.1;
        break;
    case 'h':
        light_speed -= 0.1;
        if(light_speed < 0.0f) {
          light_speed = 0.0f;
        }
        break;
    }
    glutPostRedisplay();
}

void idle ( void )
{
    camera_rotation += camera_speed;
    light_rotation += light_speed;

    glutPostRedisplay();
}

void
menuCallback(int value) {
    if(value == 4) {
        glutIdleFunc(NULL);
    }
    else if(value == 3) {
        glutIdleFunc(idle);
    }
    else if(value == 5) {
        list<Model*>::iterator iterator = models.begin();
        while(iterator != models.end()) {
            (*iterator)->initialize();
            (*iterator)->applyMaterial(pink_material_diffuse, pink_material_ambient, pink_material_specular, pink_material_shininess);
            iterator++;
        }
        glutPostRedisplay();
    }
    else if(value == 6) {
        list<Model*>::iterator iterator = models.begin();
        while(iterator != models.end()) {
            (*iterator)->initialize();
            (*iterator)->applyMaterial(blue_material_diffuse, blue_material_ambient, blue_material_specular, blue_material_shininess);
            iterator++;
        }
        glutPostRedisplay();
    }
    else if(value == 7) {
        list<Model*>::iterator iterator = models.begin();
        while(iterator != models.end()) {
            (*iterator)->initialize();
            (*iterator)->applyMaterial(stone_material_diffuse, stone_material_ambient, stone_material_specular, stone_material_shininess);
            iterator++;
        }
        glutPostRedisplay();
    }
    else if(value == 8) {
        list<Model*>::iterator iterator = models.begin();
        while(iterator != models.end()) {
            (*iterator)->changeShader(0);
            iterator++;
        }
        glutPostRedisplay();
    }
    else if(value == 9) {
        list<Model*>::iterator iterator = models.begin();
        while(iterator != models.end()) {
            (*iterator)->changeShader(1);
            iterator++;
        }
        glutPostRedisplay();
    }
    else {
        mode = value;
        glutPostRedisplay();
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    printf("USAGE:\n");
    printf("To run this program type %s\n", argv[0]);
    printf("The program opens one window and draws the shapes required for assignment 9 part 2.\n");
    printf("Keys: \n");
    printf("q - Increase camera height\n");
    printf("a - Decrease camera height\n");
    printf("w - Increase camera radius\n");
    printf("s - Decrease camera radius\n");
    printf("e - Increase camera speed\n");
    printf("d - Decrease camera speed\n");
    printf("r - Increase light height\n");
    printf("f - Decrease light height\n");
    printf("t - Increase light radius\n");
    printf("g - Decrease light radius\n");
    printf("y - Increase light speed\n");
    printf("h - Decrease light speed\n");
    printf("Mouse: \n");
    printf("Right click to access the projection, camera start/stop menu, materials, and shading menus\n");
    printf("Light Positions: \n");
    printf("Static Light 1: At ( 0.0, 2.0, 0.0) diffuse (0.5, 0.5, 0.5) ambient (0.2, 0.2, 0.2) specular (0.5, 0.5, 0.5)\n");
    printf("Static Light 2: At ( 0.0, -2.0, 0.0) diffuse (0.5, 0.5, 0.5) ambient (0.2, 0.2, 0.2) specular (0.5, 0.5, 0.5)\n");
    printf("Dynamic Light: Starts At ( 0.0, 0.0, 2.0) Rotates At Start At 0.1f diffuse (0.5, 0.5, 0.5) ambient (0.2, 0.2, 0.2) specular (0.5, 0.5, 0.5)\n");
    printf("Materials: \n");
    printf("White Material: Diffuse (1.0, 1.0, 1.0) Ambient (1.0, 1.0, 1.0) Specular (1.0, 1.0, 1.0) Shininess 5.0 \n");
    printf("Blue Latex Material: Diffuse (0.0, 0.8, 0.0) Ambient (0.0, 0.0, 0.5) Specular (0.0, 0.0, 1.0) Shininess 50.0 \n");
    printf("Stone Material: Diffuse (0.2, 0.2, 0.2) Ambient (0.5, 0.5, 0.5) Specular (0.8, 0.8, 0.8) Shininess 10.0 \n");
    printf("Note: The bunny should be textured with a cloud-like pattern. You can change the material but the cloud \n");
    printf("pattern looks best on the pure white material. \n");

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 500, 500 );

    glewExperimental=GL_TRUE; 

    mainWindow = glutCreateWindow( "Assignment 9 part 2" );
    glewInit();   

    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_3D);
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    glEnable(GL_CULL_FACE);

    Model::Initialize();

    glutCreateMenu(menuCallback);
    glutAddMenuEntry("Parallel", 1);
    glutAddMenuEntry("Perspective", 2);
    glutAddMenuEntry("Camera Start", 3);
    glutAddMenuEntry("Camera Stop", 4);
    glutAddMenuEntry("White Material", 5);
    glutAddMenuEntry("Blue Latex Material", 6);
    glutAddMenuEntry("Stone Material", 7);
    glutAddMenuEntry("Phong Shading Model", 8);
    glutAddMenuEntry("Gouraud Shading Model", 9);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
