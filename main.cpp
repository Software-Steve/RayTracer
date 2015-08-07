/*
 * This is the main file. It sets up the window using glut and initializes OpenGL extensions using glew
 *
 * We use freeglut, an open-source library inspired from the original glut. GLUT (and freeglut) is a cross-platform OpenGL library for opening windows and other UI elements
 * It is by no means a full-fledged powerful UI framework, but it is very simple to use, and is cross-platform.
 * Feel free to use and modify this file for every project.
 *
 * glut and indeed opengl, were written for C, and as such have no inherent support for objects. That is why this file looks like a C program.
 *
 * In order to initialize OpenGL extensions to use OpenGL 4, we use glew. This simplifies setting it up for every program.
 **/


#include <QCoreApplication> //the Qt include, not being used right now for anything

//You will find this header until C:\QTFiles, so include that path in the project file
#include <GL/glew.h> //the header for the glew library
#include <GL/freeglut.h> //the header for the glut library
#include "View.h"
#include <iostream>
using namespace std;

/* function prototypes */
void init(); //the function that will be used to initialize stuff
void display(); //the function is called every time the screen is to be refreshed
void keyboard(unsigned char key,int x,int y);
void resize(int w,int h); //the function that is called every time the window is manually or automatically resized
void idle();

View v; //an object to our View class that encapsulates everything that we do.

typedef struct
{
    int frames;
    int timeBase;
    int timeElapsed;
    float framerate;
} FrameRateMeasurementInfo;

enum MENUCODES {FILE_OPEN};
string filename;
float time;

FrameRateMeasurementInfo fpsInfo;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv); //standard Qt stuff, do not change



    //initialize the GLUT environment, standard for all glut programs
    glutInit(&argc,argv);

    /*
     *we want a glut window that is
     *  1. double-buffered (for flicker-less rendering)
     *  2. supports red, green, blue and alpha (transparency)
     *  3. Uses a depth buffer, essentially for any kind of 3D drawing
     */
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA | GLUT_DEPTH);

    //the initial size of the window
    glutInitWindowSize(600,600);

    //the position of the top left corner of the window on the screen
    glutInitWindowPosition(400,400);

    //we want a window that is capable of rendering OpenGL 4.0 commands
    glutInitContextVersion(4,0);

    //OpenGL 4.0 contains a core profile and a compatibility profile. Since we are learning to use purely OpenGL 4.0, we will use core profile
    glutInitContextProfile(GLUT_CORE_PROFILE);

    //this call finally creates and shows the window with the provided title.
    //Important: No OpenGL command before this line will work!.
    glutCreateWindow("Raytracer");

    //glew currently has a bug that causes problems with 4.0. Setting this glew flag is the temporary solution for it
    glewExperimental = GL_TRUE;

    //initialize glew which initializes all the OpenGL extensions.
    if (glewInit()!=GLEW_OK)
    {
        cerr << "Unable to initialize GLEW...exiting" << endl;
        exit(1);
    }


    //initialize stuff. This will likely change with every program.
    init();

    //tell glut which function to call in order to draw the screen
    //You can name the drawing function whatever you want, so long as it does not take any parameters and returns void
    glutDisplayFunc(display);

    //tell glut which function to call if the window resizes
    //You can name the resizing function whatever you want, so long as it takes two integer parameters (width and height of the resized window) and returns void
    glutReshapeFunc(resize);

    glutKeyboardFunc(keyboard);

    //tell glut which function to call if it is sitting idle. This is useful for animation
    glutIdleFunc(idle);


    //this starts the main (and infinite) loop that listens to events, refreshes, whatever
    glutMainLoop();

    //at this step you would clean up any memory, etc.

    return a.exec(); //standard Qt stuff, do not change
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'r':
    case 'R':
        v.initRaytraceMode();
        glutPostRedisplay();
        break;
    case 'o':
    case 'O':
        v.initOpenGLMode();
        glutPostRedisplay();
        break;
    }
}

/*
 * This will be our display function. Whenever glut needs to redraw the window, it will call this function.
 * The name of this function (display) is of no consequence: you can name it whatever you want, so long as the constraints below are obeyed.
 * Thus all your rendering code should be in this function, or should be called from this function.
 *
*/

void display()
{
    string frameRateString = "Frame rate: ";

    //clear the screen with above background color, and the depth buffer with all 0's
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //enable depth testing, needed to make occlusions correct
    glEnable(GL_DEPTH_TEST);

    //delegate to our view class
    v.draw();



    //swap buffers, since we are using double buffering
    glutSwapBuffers();
}

void idle()
{
    if (time>7.2)
    {
        time = time - 7.2f;
    }
    else
    {
        time += 0.001;
    }

    fpsInfo.frames++;
    fpsInfo.timeElapsed = glutGet(GLUT_ELAPSED_TIME);

    if (fpsInfo.timeElapsed - fpsInfo.timeBase > 1000) {
        fpsInfo.framerate = fpsInfo.frames*1000.0/(fpsInfo.timeElapsed-fpsInfo.timeBase);
        fpsInfo.timeBase = fpsInfo.timeElapsed;
        fpsInfo.frames = 0;
    }


    v.animate(time);

    cout << "Frame rate: " << fpsInfo.framerate << "\r";
    glutPostRedisplay();
}


/*
 * This function will be called by glut whenever the window resizes. This happens initially when it creates the window, and later if the user manually resizes the window or maximizes it.
 * The name of this function (resize) is of no consequence: you can name it whatever you want, so long as the constraints below are obeyed.
 * This function must take two integers as parameters, the width and height of the resized window in that order
 * This function must return void.
 **/
void resize(int w,int h)
{
    //delegate to our view class.
    v.resize(w,h);

    //sets the viewport to cover the entire area of the resized window
    //glViewport(leftx,topy,width,height)
    glViewport(0,0,w,h);

    /*
     *whenver you need glut to refresh the window, use this command
     *This is a non-blocking call. That is, it will tell glut to refresh the window but won't wait for it
     *to actually draw.
     *
     *GLUT too will not drop everything it is doing to refresh. It will remember this command in a queue.
     *And will get to it as soon as it is done with whatever else it is doing (if it is doing something else).
     *
     * This is to prevent GLUT from being overwhelmed with too many refresh calls. For example this happens
     * in the case where the time between two refresh calls is lesser than
     * the time it takes to draw the screen one time.
     */
    glutPostRedisplay();
}

void init()
{

    //sets the background color to be black, does not actually clear the window
    glClearColor(0,0,0,1);
    //delegate to our view class to do all the initializing
    v.initialize();
    //char s[50];
    string s;

  //  cout << "Enter the name of the input file: ";
  //  cin >> s;
    s = "input.txt";
  //  s = "face-hierarchy-lights-textures.txt";
    v.openFile(s);
    //set the time t to 0
    time = 0;
    fpsInfo.timeBase = fpsInfo.timeElapsed = 0;
    fpsInfo.frames = 0;
}

