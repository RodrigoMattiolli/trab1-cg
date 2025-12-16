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
    
    // CAMPOS DE DANO
    bool isHit;
    float hitTimer;
    float blinkTimer;
    bool visible;
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
    float arenaR; 
    float p1StartX, p1StartY;
    float p2StartX, p2StartY;


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
    
    bool CircleCollision(float x1,float y1,float r1, float x2,float y2,float r2);
    bool PlayerCollidesArena(const Player &p, float nx, float ny);
    bool PlayerCollidesObstacles(const Player &p, float nx, float ny);
    bool PlayersOverlap(float nx1,float ny1,float r1, float nx2, float ny2, float r2);
    
    bool CheckProjectileCollisions(Projectile &pr);
    
    void TryMove(Player &p, float dx, float dy, const Player &otherP);

    void DrawText(const char *text, float x, float y);
    void ProcessInputForPlayer(Player &pl, int playerIdx, double dt);
};

extern Game *g_game; 

#endif