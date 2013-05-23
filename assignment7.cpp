// First new GL program
// Just makes a red triangle

#include <cstdlib>
#include <list>

#include "Angel.h"
#include "BezierPatch.h"
#include "BezierPatchParser.h"

using namespace std;

//--------------------------------------------------------------------------

int mainWindow;
int mode = 1;

vec4 camera_position = vec4(0.0f, 0.0f, 1.0f, 1.0);
vec4 camera_up = vec4(0.0, 1.0, 0.0, 0.0);
vec4 camera_look_at = vec4(0.0, 0.0, 0.0, 1.0);

vec4 light_position = vec4(0.0f, 2.0f, 0.0f, 1.0f);
vec3 light_diffuse = vec3(0.5f, 0.5f, 0.5f);
vec3 light_ambient = vec3(0.2f, 0.2f, 0.2f);
vec3 light_specular = vec3(0.5f, 0.5f, 0.5f);

GLfloat light_radius = -2.0f;
GLfloat light_height = 0.0f;
GLfloat light_rotation = 0.0f;
GLfloat light_speed = 0.1f;

vec3 pink_material_diffuse = vec3(1.0f, 0.8f, 0.0f);
vec3 pink_material_ambient = vec3(1.0f, 0.0f, 1.0f);
vec3 pink_material_specular = vec3(1.0f, 0.0f, 1.0f);
GLfloat pink_material_shininess = 5.0f;

vec3 blue_material_diffuse = vec3(0.0f, 0.8f, 0.0f);
vec3 blue_material_ambient = vec3(0.0f, 0.0f, 0.5f);
vec3 blue_material_specular = vec3(0.0f, 0.0f, 1.0f);
GLfloat blue_material_shininess = 50.0f;

vec3 stone_material_diffuse = vec3(0.2f, 0.2f, 0.2f);
vec3 stone_material_ambient = vec3(0.5f, 0.5f, 0.5f);
vec3 stone_material_specular = vec3(0.8f, 0.8f, 0.8f);
GLfloat stone_material_shininess = 10.0f;

BezierPatch * patch;

void
init( void )
{
    char * inFile = (char*)"patchPoints.txt";
    BezierPatchParser parser(inFile);

    patch = parser.parse();
    patch->initialize();
    patch->applyMaterial(pink_material_diffuse, pink_material_ambient, pink_material_specular, pink_material_shininess);
}

void
dealloc( void ) {
    BezierPatch::Deinitialize();
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClearColor( 0.1, 0.1, 0.1, 1.0 ); // black background
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

    mat4 modelView = LookAt(camera_position, camera_look_at, camera_up);
    mat4 projection;

    if(mode == 1) {
        projection = Ortho(-2.0f, 8.0f, -2.0f, 8.0f, -100.0f, 100.0f);
    }
    else {
        projection = Perspective(45.0f, 1.0f, 0.5f, 100.0f);
    }

    patch->draw(modelView, projection, camera_position, light_position, light_diffuse, light_ambient, light_specular);

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
        patch->changePointBy(0.1f, 0.0f, 0.0f);
        break;
    case 'w':
        patch->changePointBy(0.0f, 0.1f, 0.0f);
        break;
    case 'e':
        patch->changePointBy(0.0f, 0.0f, 0.1f);
        break;
    case 'a':
        patch->changePointBy(-0.1f, 0.0f, 0.0f);
        break;
    case 's':
        patch->changePointBy(0.0f, -0.1f, 0.0f);
        break;
    case 'd':
        patch->changePointBy(0.0f, 0.0f, -0.1f);
        break;
    }
    glutPostRedisplay();
}

void
specialKeyboard( int key, int x, int y )
{
    switch ( key ) {
    case GLUT_KEY_UP:
        patch->selectPointToUp();
        break;
    case GLUT_KEY_DOWN:
        patch->selectPointToDown();
        break;
    case GLUT_KEY_LEFT:
        patch->selectPointToLeft();
        break;
    case GLUT_KEY_RIGHT:
        patch->selectPointToRight();
        break;
    }
    glutPostRedisplay();
}

void idle ( void )
{
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
        patch->applyMaterial(pink_material_diffuse, pink_material_ambient, pink_material_specular, pink_material_shininess);
        glutPostRedisplay();
    }
    else if(value == 6) {
        patch->applyMaterial(blue_material_diffuse, blue_material_ambient, blue_material_specular, blue_material_shininess);
        glutPostRedisplay();
    }
    else if(value == 7) {
        patch->applyMaterial(stone_material_diffuse, stone_material_ambient, stone_material_specular, stone_material_shininess);
        glutPostRedisplay();
    }
    else if(value == 8) {
        patch->changeShader(0);
        glutPostRedisplay();
    }
    else if(value == 9) {
        patch->changeShader(1);
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
    printf("The program opens one window and draws the shapes required for assignment 5.\n");
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
    printf("Pink Material: Diffuse (1.0, 0.8, 0.0) Ambient (1.0, 0.0, 1.0) Specular (1.0, 0.0, 1.0) Shininess 5.0 \n");
    printf("Blue Latex Material: Diffuse (0.0, 0.8, 0.0) Ambient (0.0, 0.0, 0.5) Specular (0.0, 0.0, 1.0) Shininess 50.0 \n");
    printf("Stone Material: Diffuse (0.2, 0.2, 0.2) Ambient (0.5, 0.5, 0.5) Specular (0.8, 0.8, 0.8) Shininess 10.0 \n");

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 500, 500 );

    glewExperimental=GL_TRUE; 

    mainWindow = glutCreateWindow( "Assignment 6" );
    glewInit();   

    glEnable(GL_DEPTH_TEST); 
  //  glEnable(GL_CULL_FACE);

    BezierPatch::Initialize();

    glutCreateMenu(menuCallback);
    glutAddMenuEntry("Parallel", 1);
    glutAddMenuEntry("Perspective", 2);
    glutAddMenuEntry("Pink Material", 5);
    glutAddMenuEntry("Blue Latex Material", 6);
    glutAddMenuEntry("Stone Material", 7);
    glutAddMenuEntry("Phong Shading Model", 8);
    glutAddMenuEntry("Gouraud Shading Model", 9);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc( specialKeyboard );
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
