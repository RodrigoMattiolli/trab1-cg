#include "game.h"
#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>

Game *g_game = nullptr;

static void displayCB(){
    if (g_game) g_game->Render();
}
static void idleCB(){
    if (g_game) g_game->Idle();
}
static void keyboardDownCB(unsigned char key, int x, int y){
    if (g_game) g_game->KeyDown(key);
}
static void keyboardUpCB(unsigned char key, int x, int y){
    if (g_game) g_game->KeyUp(key);
}
static void passiveMouseCB(int x, int y){
    if (g_game) g_game->MouseMove(x,y);
}
static void mouseCB(int button, int state, int x, int y){
    if (g_game) g_game->MouseClick(button,state,x,y);
}
static void reshapeCB(int w, int h){
    if (g_game) g_game->Resize(w,h);
}

int main(int argc, char** argv){
    if (argc < 2){
        printf("Usando: %s arena.svg\n", argv[0]);
        return 1;
    }
    g_game = new Game();
    if (!g_game->LoadArena(argv[1])){
        printf("Falha para carregar arena.svg: %s\n", argv[1]);
        return 1;
    }
    g_game->InitGLWindow(argc, argv);

    // register callbacks
    glutDisplayFunc(displayCB);
    glutIdleFunc(idleCB);
    glutKeyboardFunc(keyboardDownCB);
    glutKeyboardUpFunc(keyboardUpCB);
    glutPassiveMotionFunc(passiveMouseCB);
    glutMouseFunc(mouseCB);
    glutReshapeFunc(reshapeCB);

    // start loop
    glutMainLoop();

    delete g_game;
    return 0;
}
