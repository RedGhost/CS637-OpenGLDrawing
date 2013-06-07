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

GLfloat camera_radius = 1.0f;
GLfloat camera_height = 0.0f;
GLfloat camera_rotation = 0.0f;

vec4 camera_position = vec4(3.0f, 3.0f, 11.0f, 1.0);
vec4 camera_up = vec4(0.0, 1.0, 0.0, 0.0);
vec4 camera_look_at = vec4(3.0, 3.0, 1.4, 1.0);

vec3 light_diffuse = vec3(0.5f, 0.5f, 0.5f);
vec3 light_ambient = vec3(0.2f, 0.2f, 0.2f);
vec3 light_specular = vec3(0.5f, 0.5f, 0.5f);

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

const int TextureSize = 64;
GLubyte textureData[TextureSize][TextureSize][3];
GLuint texture;

BezierPatch * patch;

void
init( void )
{
    // Create a checkerboard pattern
    for ( int i = 0; i < 64; i++ ) {
        for ( int j = 0; j < 64; j++ ) {
            GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8)  == 0)) * 255;
            textureData[i][j][0]  = c;
            textureData[i][j][1]  = c;
            textureData[i][j][2]  = c;
        }
    }

    // Initialize texture objects
    glGenTextures( 1, &texture );

    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0,
		  GL_RGB, GL_UNSIGNED_BYTE, textureData );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture );

    char * inFile = (char*)"patchPoints.txt";
    BezierPatchParser parser(inFile);

    patch = parser.parse();
    patch->initialize();
    patch->applyMaterial(pink_material_diffuse, pink_material_ambient, pink_material_specular, pink_material_shininess);
    patch->applyTexture(0);
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
    glPointSize( 8.0f );

    vec4 current_camera_position = RotateY(camera_rotation) * Translate(0.0f, camera_height, -camera_radius) * camera_position;

    mat4 modelView = LookAt(current_camera_position, camera_look_at, camera_up);
    mat4 projection = Perspective(45.0f, 1.0f, 0.5f, 100.0f);

    patch->draw(modelView, projection, current_camera_position, current_camera_position, light_diffuse, light_ambient, light_specular);

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
    case 'y':
        camera_height += 0.1;
        break;
    case 'h':
        camera_height -= 0.1;
        break;
    case 'u':
        camera_radius -= 0.1;
        if(camera_radius < 0.0f) {
          camera_radius = 0.0f;
        }
        break;
    case 'j':
        camera_radius += 0.1;
        break;
    case 'i':
        camera_rotation += 1;
        break;
    case 'k':
        camera_rotation -= 1;
        break;
    case 'o':
        patch->changeResolutionBy(1);
        break;
    case 'l':
        patch->changeResolutionBy(-1);
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

void
menuCallback(int value) {
    if(value == 5) {
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
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    printf("USAGE:\n");
    printf("To run this program type %s\n", argv[0]);
    printf("The program opens one window and draws the shapes required for assignment 9 part 1.\n");
    printf("Keys: \n");
    printf("Arrow keys - Navigate between control points\n");
    printf("q - Increase x-value of control point\n");
    printf("a - Decrease x-value of control point\n");
    printf("w - Increase y-value of control point\n");
    printf("s - Decrease y-value of control point\n");
    printf("e - Increase z-value of control point\n");
    printf("d - Decrease z-value of control point\n");
    printf("y - Increase camera height\n");
    printf("h - Decrease camera height\n");
    printf("u - Increase camera radius\n");
    printf("j - Decrease camera radius\n");
    printf("i - Increase rotation around the y-axis\n");
    printf("k - Decrease rotation around the y-axis\n");
    printf("o - Increase resolution by 1 (Max 20)\n");
    printf("l - Decrease resolution by 1 (Min 1)\n");
    printf("esc - Quit application\n");
    printf("Mouse: \n");
    printf("Right click to access the materials\n");
    printf("Light Positions: \n");
    printf("There is one light at the position of the camera with diffuse (0.5, 0.5, 0.5) ambient (0.2, 0.2, 0.2) specular (0.5, 0.5, 0.5)\n");
    printf("Materials: \n");
    printf("Pink Material: Diffuse (1.0, 0.8, 0.0) Ambient (1.0, 0.0, 1.0) Specular (1.0, 0.0, 1.0) Shininess 5.0 \n");
    printf("Blue Latex Material: Diffuse (0.0, 0.8, 0.0) Ambient (0.0, 0.0, 0.5) Specular (0.0, 0.0, 1.0) Shininess 50.0 \n");
    printf("Stone Material: Diffuse (0.2, 0.2, 0.2) Ambient (0.5, 0.5, 0.5) Specular (0.8, 0.8, 0.8) Shininess 10.0 \n");
    printf("NOTE: The bezier patch should have a checkerboard pattern applied to it. \n");
    printf("NOTE: press escape to quit, killing the process will make the makefile not run part 2. \n");

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 500, 500 );

    glewExperimental=GL_TRUE; 

    mainWindow = glutCreateWindow( "Assignment 7" );
    glewInit();   

    glEnable(GL_DEPTH_TEST); 

    BezierPatch::Initialize();

    glutCreateMenu(menuCallback);
    glutAddMenuEntry("Pink Material", 5);
    glutAddMenuEntry("Blue Latex Material", 6);
    glutAddMenuEntry("Stone Material", 7);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc( specialKeyboard );

    glutMainLoop();
    return 0;
}
