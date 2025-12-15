// #ifndef GAME_H
// #define GAME_H

// #include <vector>
// #include <string>
// #include "svgparser.h"

// // forward for GL types
// #include <GL/gl.h>

// struct Obstacle {
//     float x,y,r;
// };

// struct Projectile {
//     float x,y;
//     float dx,dy; // normalized direction
//     float speed;
//     float travelled;
//     bool alive;
//     int owner; // 1 or 2
// };

// struct Player {
//     float x,y;      // center (head center)
//     float angle;    // orientation in degrees (0 = right)
//     float headR;    // radius of head
//     float armAngle; // relative to player forward, degrees
//     int lives;
//     bool aliveFlag;
//     // animation
//     bool legForwardLeft; // which foot forward
//     float legTimer;
// };

// class Game {
// public:
//     Game();
//     bool LoadArena(const std::string &svgPath);
//     void InitGLWindow(int argc, char** argv);
//     void Run(); // starts glut loop

//     // rendering and update called by main
//     void Render();
//     void Idle();
//     void Resize(int w, int h);

//     // input
//     void KeyDown(unsigned char key);
//     void KeyUp(unsigned char key);
//     void SpecialDown(int key);
//     void MouseMove(int x, int y);
//     void MouseClick(int button, int state, int x, int y);

//     // helper
//     void Reset();
// private:
//     // world
//     float arenaX, arenaY, arenaR;
//     std::vector<Obstacle> obstacles;
//     Player p1, p2;
//     std::vector<Projectile> projectiles;
//     int width, height; // window pixels (500)
//     double previousTime; // milliseconds
//     float viewHalf;

//     // keyboard state
//     bool keyState[256];

//     // internal helpers
//     void Update(double dt); // dt in seconds
//     void DrawCircle(float cx, float cy, float r);
//     void DrawPlayer(const Player &p, int playerId);
//     void DrawObstacles();
//     void FireProjectile(int owner, float x, float y, float ang);
//     bool CircleCollision(float x1,float y1,float r1, float x2,float y2,float r2);
//     bool PlayerCollidesArena(const Player &p, float nx, float ny);
//     bool PlayerCollidesObstacles(const Player &p, float nx, float ny);
//     bool PlayersOverlap(float nx1,float ny1,float r1, float nx2, float ny2, float r2);
//     void CheckProjectileCollisions(Projectile &pr);
//     void DrawText(const char *text, float x, float y);
//     // input helpers for each player
//     void ProcessInputForPlayer(Player &pl, int playerIdx, double dt);
// };

// extern Game *g_game; // for C-style glut callbacks

// #endif

//Versão do Gemini (LEMBRAR DE APAGARRRRRRR)
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "svgparser.h"
#include <GL/gl.h>

struct Obstacle {
    float x,y,r;
};

struct Projectile {
    float x,y;
    float dx,dy; 
    float speed;
    float travelled;
    bool alive;
    int owner; 
};

struct Player {
    float x,y;      
    float angle;    
    float headR;    
    float armAngle; 
    int lives;
    bool aliveFlag;
    bool legForwardLeft; 
    float legTimer;
};

class Game {
public:
    Game();
    bool LoadArena(const std::string &svgPath);
    void InitGLWindow(int argc, char** argv);
    void Run(); 
    void Render();
    void Idle();
    void Resize(int w, int h);
    void KeyDown(unsigned char key);
    void KeyUp(unsigned char key);
    void MouseMove(int x, int y);
    void MouseClick(int button, int state, int x, int y);
    void Reset();

private:
    float arenaR; // Apenas o raio importa, o centro é sempre (0,0)
    
    // Posições iniciais (Normalizadas relativo ao centro da arena)
    float p1StartX, p1StartY, p1StartR;
    float p2StartX, p2StartY, p2StartR;

    std::vector<Obstacle> obstacles;
    Player p1, p2;
    std::vector<Projectile> projectiles;
    int width, height; 
    double previousTime; 

    bool keyState[256];

    void Update(double dt);
    void DrawCircle(float cx, float cy, float r);
    void DrawPlayer(const Player &p, int playerId);
    void DrawObstacles();
    
    // Placeholders Tópico 2
    bool CircleCollision(float x1,float y1,float r1, float x2,float y2,float r2);
    bool PlayerCollidesArena(const Player &p, float nx, float ny);
    bool PlayerCollidesObstacles(const Player &p, float nx, float ny);
    bool PlayersOverlap(float nx1,float ny1,float r1, float nx2, float ny2, float r2);
    void CheckProjectileCollisions(Projectile &pr);
    void DrawText(const char *text, float x, float y);
    void ProcessInputForPlayer(Player &pl, int playerIdx, double dt);
};

extern Game *g_game; 

#endif