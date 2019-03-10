#include <GL/glew.h>

#include <GL/freeglut.h>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "scene.h"

std::unique_ptr<ou::Scene> scene;

namespace ou {

class Callbacks {
public:
    static void renderScene(void)
    {
        scene->render();
        glutSwapBuffers();

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << gluErrorString(err);
        }

        glutPostRedisplay();
    }

    static void keyboardDown(unsigned char key, int x, int y)
    {
        scene->keyDown(key);
    }

    static void keyboardUp(unsigned char key, int x, int y)
    {
        scene->keyUp(key);
    }

    static void mouseMove(int x, int y)
    {
        scene->mouseMove(x, y);
    }

    static void mouseEntry(int state)
    {
        if (state == GLUT_ENTERED) {
            scene->mouseEnter();
        }
    }
};
}

int main(int argc, char* argv[])
{
    // init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 480);
    glutCreateWindow("OUGL");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW\n";
        return 1;
    }

    // register callbacks
    glutKeyboardFunc(ou::Callbacks::keyboardDown);
    glutKeyboardUpFunc(ou::Callbacks::keyboardUp);
    glutDisplayFunc(ou::Callbacks::renderScene);
    glutPassiveMotionFunc(ou::Callbacks::mouseMove);
    glutEntryFunc(ou::Callbacks::mouseEntry);

    try {
        scene = std::make_unique<ou::Scene>();

        // enter GLUT event processing cycle
        glutMainLoop();
    } catch (std::exception const& e) {
        std::cerr << "Exception caught while running program: " << e.what() << "\n";
    }

    return 0;
}
