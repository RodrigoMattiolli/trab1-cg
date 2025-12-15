// #include "game.h"
// #include <GL/glut.h>
// #include <cmath>
// #include <cstring>

// #define CEDILLA 186
// extern Game *g_game;

// Game::Game(){
//     g_game = this;

//     width = height = 500;
//     viewHalf = 250.0f;

//     arenaX = 0;
//     arenaY = 0;
//     arenaR = 250;

//     previousTime = glutGet(GLUT_ELAPSED_TIME);

//     for(int i=0;i<256;i++)
//         keyState[i] = false;

//     Reset();
// }

// void Game::Reset(){
//     p1 = { -80, 0,   0, 0, 15, 3, true, 0, true };
//     p2 = {  80, 0, 180, 0, 15, 3, true, 0, true };

//     projectiles.clear();
// }

// bool Game::LoadArena(const std::string &){
//     return true;
// }

// void Game::InitGLWindow(int argc, char **argv){
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//     glutInitWindowSize(width, height);
//     glutCreateWindow("Trabalho CG");

//     glClearColor(0,0,0,1);
//     Resize(width,height);

//     previousTime = glutGet(GLUT_ELAPSED_TIME);
// }

// void Game::Resize(int w,int h){
//     width = w;
//     height = h;

//     glViewport(0,0,w,h);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();

//     float aspect = (float)w/(float)h;

//     if(aspect >= 1.0f)
//         glOrtho(-arenaR*aspect, arenaR*aspect, -arenaR, arenaR, -1,1);
//     else
//         glOrtho(-arenaR, arenaR, -arenaR/aspect, arenaR/aspect, -1,1);

//     glMatrixMode(GL_MODELVIEW);
// }

// void Game::Render(){
//     glClear(GL_COLOR_BUFFER_BIT);
//     glLoadIdentity();

//     glColor3f(0,0,1);
//     DrawCircle(0,0,arenaR);
//     DrawObstacles();
//     DrawPlayer(p1,1);
//     DrawPlayer(p2,2);

//     glColor3f(1,1,1);
//     for(auto &t: projectiles)
//         if(t.alive)
//             DrawCircle(t.x,t.y,3);

//     glutSwapBuffers();
// }

// void Game::DrawCircle(float cx,float cy,float r){
//     glBegin(GL_POLYGON);
//     for(int i=0;i<64;i++){
//         float a = 2*M_PI*i/64;
//         glVertex2f(cx+cos(a)*r, cy+sin(a)*r);
//     }
//     glEnd();
// }

// void Game::DrawObstacles()
// {
//     glColor3f(0.4f, 0.4f, 0.4f);

//     for(const Obstacle &o : obstacles){
//         glBegin(GL_QUADS);
//             glVertex2f(o.x - o.w/2, o.y - o.h/2);
//             glVertex2f(o.x + o.w/2, o.y - o.h/2);
//             glVertex2f(o.x + o.w/2, o.y + o.h/2);
//             glVertex2f(o.x - o.w/2, o.y + o.h/2);
//         glEnd();
//     }
// }


// void Game::DrawPlayer(const Player &p, int id)
// {
//     glPushMatrix();

//     // Posição do jogador
//     glTranslatef(p.x, p.y, 0.0f);
//     glRotatef(p.angle, 0, 0, 1);

//     // ===== CORES =====
//     if(id == 1) glColor3f(0, 1, 0);   // Player 1: verde
//     else        glColor3f(1, 0, 0);   // Player 2: vermelho

//     // ===== CORPO =====
//     glBegin(GL_QUADS);
//         glVertex2f(-10, 0);
//         glVertex2f( 10, 0);
//         glVertex2f( 10, 40);
//         glVertex2f(-10, 40);
//     glEnd();

//     // ===== CABEÇA =====
//     glPushMatrix();
//         glTranslatef(0, 55, 0);
//         DrawCircle(0, 0, 15);
//     glPopMatrix();

//     // ===== CANHÃO / BRAÇO =====
//     glPushMatrix();
//         glTranslatef(0, 40, 0);
//         glRotatef(p.armAngle, 0, 0, 1);
//         glBegin(GL_QUADS);
//             glVertex2f(0, -3);
//             glVertex2f(30, -3);
//             glVertex2f(30,  3);
//             glVertex2f(0,  3);
//         glEnd();
//     glPopMatrix();

//     // ===== PERNAS =====
//     glColor3f(0.2f, 0.2f, 0.2f);
//     glBegin(GL_LINES);
//         glVertex2f(-5, 0);
//         glVertex2f(-10, -20);

//         glVertex2f(5, 0);
//         glVertex2f(10, -20);
//     glEnd();

//     glPopMatrix();
// }



// void Game::MouseMove(int x,int y){
//     float nx = (float)x/width;
//     float ny = 1.0f - (float)y/height;

//     float aspect = (float)width/height;

//     float left  = (aspect>=1)? -arenaR*aspect : -arenaR;
//     float right = (aspect>=1)?  arenaR*aspect :  arenaR;
//     float bot   = (aspect>=1)? -arenaR        : -arenaR/aspect;
//     float top   = (aspect>=1)?  arenaR        :  arenaR/aspect;

//     float wx = left + nx*(right-left);
//     float wy = bot  + ny*(top-bot);

//     float dx = wx - p1.x;
//     float dy = wy - p1.y;
//     p1.angle = atan2(dy,dx)*180/M_PI;
// }

// void Game::MouseClick(int btn,int state,int,int){
//     if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
//         Projectile t;
//         t.x = p1.x;
//         t.y = p1.y;
//         float a = p1.angle*M_PI/180;
//         t.dx = cos(a);
//         t.dy = sin(a);
//         t.speed = 200;
//         t.travelled = 0;
//         t.alive = true;
//         t.owner = 1;
//         projectiles.push_back(t);
//     }
// }

// void Game::KeyDown(unsigned char key){
//     keyState[key] = true;

//     // Player 2 fire
//     if(key == '5'){
//         Projectile t;
//         t.x = p2.x;
//         t.y = p2.y;
//         float a = p2.angle*M_PI/180;
//         t.dx = cos(a);
//         t.dy = sin(a);
//         t.speed = 200;
//         t.travelled = 0;
//         t.alive = true;
//         t.owner = 2;
//         projectiles.push_back(t);
//     }

//     if(key == 'r' || key == 'R')
//         Reset();
// }

// void Game::KeyUp(unsigned char key){
//     keyState[key] = false;
// }

// void Game::Update(double dt){
//     float moveSpeed = 100 * dt;
//     float rotSpeed  = 120 * dt;

//     // PLAYER 1
//     if(keyState['w']){
//         p1.x += cos(p1.angle*M_PI/180)*moveSpeed;
//         p1.y += sin(p1.angle*M_PI/180)*moveSpeed;
//     }
//     if(keyState['s']){
//         p1.x -= cos(p1.angle*M_PI/180)*moveSpeed;
//         p1.y -= sin(p1.angle*M_PI/180)*moveSpeed;
//     }
//     if(keyState['a']) p1.angle += rotSpeed;
//     if(keyState['d']) p1.angle -= rotSpeed;

//     // PLAYER 2
//     if(keyState['o']){
//         p2.x += cos(p2.angle*M_PI/180)*moveSpeed;
//         p2.y += sin(p2.angle*M_PI/180)*moveSpeed;
//     }
//     if(keyState['l']){
//         p2.x -= cos(p2.angle*M_PI/180)*moveSpeed;
//         p2.y -= sin(p2.angle*M_PI/180)*moveSpeed;
//     }
//     if(keyState['k']) p2.angle += rotSpeed;
//     if(keyState[CEDILLA]) p2.angle -= rotSpeed;

//     // Tiros
//     for(auto &t : projectiles){
//         if(!t.alive) continue;
//         t.x += t.dx * t.speed * dt;
//         t.y += t.dy * t.speed * dt;
//         if(sqrt(t.x*t.x + t.y*t.y) > arenaR)
//             t.alive = false;
//     }
// }

// void Game::Idle(){
//     int now = glutGet(GLUT_ELAPSED_TIME);
//     double dt = (now - previousTime)/1000.0;
//     previousTime = now;

//     Update(dt);
//     glutPostRedisplay();
// }

// void Game::DrawText(const char *,float,float){}


#include "game.h"
#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <iostream>

Game *g_game = nullptr;

static double nowTime()
{
    return glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

Game::Game()
{
    std::memset(keyState, 0, sizeof(keyState));
    width = height = 500;
    viewHalf = 250.0f;
    previousTime = nowTime();
}

bool Game::LoadArena(const std::string &svgPath)
{
    SvgParser parser;
    if(!parser.load(svgPath)) return false;

    arenaX = parser.arena.cx;
    arenaY = parser.arena.cy;
    arenaR = parser.arena.r;

    obstacles.clear();
    for(const auto &c : parser.circles){
        if(!c.isArena){
            Obstacle o;
            o.x = c.cx;
            o.y = c.cy;
            o.r = c.r;
            obstacles.push_back(o);
        }
    }

    Reset();
    return true;
}

void Game::InitGLWindow(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("T2D");

    glClearColor(1,1,1,1);

    glutDisplayFunc([](){ if(g_game) g_game->Render(); });
    glutIdleFunc([](){ if(g_game) g_game->Idle(); });
    glutReshapeFunc([](int w,int h){ if(g_game) g_game->Resize(w,h); });

    glutKeyboardFunc([](unsigned char k,int,int){
        if(g_game) g_game->KeyDown(k);
    });
    glutKeyboardUpFunc([](unsigned char k,int,int){
        if(g_game) g_game->KeyUp(k);
    });

    glutSpecialFunc([](int k,int,int){
        if(g_game) g_game->SpecialDown(k);
    });

    glutMainLoop();
}

void Game::Run(){}

void Game::Reset()
{
    p1.x = arenaX - arenaR/2;
    p1.y = arenaY;
    p1.angle = 0;
    p1.headR = 15;
    p1.lives = 3;
    p1.aliveFlag = true;

    p2.x = arenaX + arenaR/2;
    p2.y = arenaY;
    p2.angle = 180;
    p2.headR = 15;
    p2.lives = 3;
    p2.aliveFlag = true;

    projectiles.clear();
}

void Game::Resize(int w,int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-viewHalf, viewHalf, -viewHalf, viewHalf, -1,1);
    glMatrixMode(GL_MODELVIEW);
}

void Game::Idle()
{
    double t = nowTime();
    double dt = t - previousTime;
    previousTime = t;

    Update(dt);
    glutPostRedisplay();
}

void Game::Update(double dt)
{
    ProcessInputForPlayer(p1,1,dt);
    ProcessInputForPlayer(p2,2,dt);

    for(auto &pr : projectiles){
        if(!pr.alive) continue;
        pr.x += pr.dx * pr.speed * dt;
        pr.y += pr.dy * pr.speed * dt;
        pr.travelled += pr.speed * dt;
        CheckProjectileCollisions(pr);
        if(pr.travelled > arenaR*2) pr.alive = false;
    }
}

void Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // arena
    glColor3f(0,0,0);
    DrawCircle(arenaX, arenaY, arenaR);

    // obstacles
    glColor3f(0.5f,0.5f,0.5f);
    for(const auto &o : obstacles)
        DrawCircle(o.x, o.y, o.r);

    DrawPlayer(p1,1);
    DrawPlayer(p2,2);

    glColor3f(0,0,0);
    for(const auto &pr : projectiles)
        if(pr.alive)
            DrawCircle(pr.x, pr.y, 4);

    glutSwapBuffers();
}

/* ====================== TÓPICO 3: COLISÕES ====================== */

bool Game::PlayerCollidesArena(const Player &p,float nx,float ny)
{
    return !CircleCollision(nx,ny,p.headR, arenaX,arenaY,arenaR);
}

bool Game::PlayerCollidesObstacles(const Player &p,float nx,float ny)
{
    for(const auto &o : obstacles)
        if(CircleCollision(nx,ny,p.headR, o.x,o.y,o.r))
            return true;
    return false;
}

void Game::CheckProjectileCollisions(Projectile &pr)
{
    for(const auto &o : obstacles)
        if(CircleCollision(pr.x,pr.y,4, o.x,o.y,o.r)){
            pr.alive = false;
            return;
        }
}

bool Game::CircleCollision(float x1,float y1,float r1,
                           float x2,float y2,float r2)
{
    float dx=x1-x2, dy=y1-y2;
    float d2 = dx*dx + dy*dy;
    float r = r1+r2;
    return d2 <= r*r;
}

/* ====================== INPUT ====================== */

void Game::KeyDown(unsigned char k){ keyState[k]=true; }
void Game::KeyUp(unsigned char k){ keyState[k]=false; }
void Game::SpecialDown(int){}

void Game::MouseMove(int,int){}
void Game::MouseClick(int,int,int,int){}

/* ====================== MOVIMENTO ====================== */

void Game::ProcessInputForPlayer(Player &pl,int id,double dt)
{
    float speed = 100 * dt;
    float ang = pl.angle * M_PI / 180.0f;

    float nx = pl.x;
    float ny = pl.y;

    if(id==1){
        if(keyState['w']){ nx+=cos(ang)*speed; ny+=sin(ang)*speed; }
        if(keyState['s']){ nx-=cos(ang)*speed; ny-=sin(ang)*speed; }
    }else{
        if(keyState['i']){ nx+=cos(ang)*speed; ny+=sin(ang)*speed; }
        if(keyState['k']){ nx-=cos(ang)*speed; ny-=sin(ang)*speed; }
    }

    if(!PlayerCollidesArena(pl,nx,ny) &&
       !PlayerCollidesObstacles(pl,nx,ny)){
        pl.x=nx;
        pl.y=ny;
    }
}

/* ====================== DESENHO ====================== */

void Game::DrawCircle(float cx,float cy,float r)
{
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<32;i++){
        float a = i*2*M_PI/32;
        glVertex2f(cx+cos(a)*r, cy+sin(a)*r);
    }
    glEnd();
}

void Game::DrawPlayer(const Player &p,int id)
{
    glPushMatrix();
    glTranslatef(p.x,p.y,0);
    glRotatef(p.angle,0,0,1);
    glColor3f(id==1,0,id==2);
    DrawCircle(0,0,p.headR);
    glPopMatrix();
}

void Game::FireProjectile(int,float,float,float){}
void Game::DrawText(const char*,float,float){}
