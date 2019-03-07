#include <GL/glew.h>

#include <GL/freeglut.h>
#include <iostream>
#include <memory>

#include "scene.h"

std::unique_ptr<ou::Scene> scene;

void renderScene(void)
{
    scene->render();
    glutSwapBuffers();

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << gluErrorString(err);
    }
}

int main(int argc, char* argv[])
{
    // init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(320, 320);
    glutCreateWindow("OUGL");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW\n";
        return 1;
    }

    // register callbacks
    glutDisplayFunc(renderScene);

    try {
        scene = std::make_unique<ou::Scene>();

        // enter GLUT event processing cycle
        glutMainLoop();
    } catch (std::exception const& e) {
        std::cerr << "Exception caught while running program: " << e.what() << "\n";
    }

    return 0;
}
