// Author:  AlexHG @ ZEN3X.COM
// License: MIT License
// Website: https://ZEN3X.COM


#include "window.hpp"

int main(int argc, char *argv[])
{
    Window *gm = new Window();
    if (gm->createSGLWindow() != 0)
        return -1;

    gm->mainLoop();
    delete gm;

    return 0;
}
