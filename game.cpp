#include "game.h"
#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <iostream>
#include <algorithm> 
#include <vector>
#include <cstdio>
#include <string>

#define CEDILLA_LOW 199
#define CEDILLA_HIGH 231

extern Game *g_game;

//Globais
static const float COMMON_RADIUS = 40.0f;
static const float PLAYER_SPREAD_FACTOR = 1.0f; 
static const float PROJECTILE_RADIUS = 7.0f;

Game::Game(){
    g_game = this;
    width = height = 500;

    arenaR = 250;
    
    
    // posx, posy, angulo, tam_cabeça, ang_braço, vidas, flag_vida, mov_pernaEsq, passos, isHit, hitTimer, blinkTimer, visivel
    p1 = { -120, 350, -90,   COMMON_RADIUS, 0, 3, true, false, 0.0f, false, 0.0f, 0.0f, true };
    p2 = {  80, 0, 90, COMMON_RADIUS, 0, 3, true, false, 0.0f, false, 0.0f, 0.0f, true };

    p1StartX = -120; p1StartY = 350;
    p2StartX = 80;  p2StartY = 0;

    previousTime = glutGet(GLUT_ELAPSED_TIME);

    for(int i=0;i<256;i++)
        keyState[i] = false;
}

// definir cores da arena e players - fill SVG
bool isColor(std::string fill, const std::string &target) {
    std::transform(fill.begin(), fill.end(), fill.begin(), ::tolower);
    if (fill.find(target) != std::string::npos) return true;
    if (target == "blue" && (fill == "#0000ff" || fill == "blue")) return true;
    if (target == "green" && (fill == "#00ff00" || fill == "#008000" || fill == "lime" || fill == "green")) return true;
    if (target == "red" && (fill == "#ff0000" || fill == "red")) return true;
    return false;
}

void Game::Reset(){
    projectiles.clear();

    // Reset Player 1
    p1.x = p1StartX;
    p1.y = p1StartY;
    p1.angle = 0;
    p1.armAngle = 0;
    p1.lives = 3;
    p1.aliveFlag = true;
    p1.isHit = false;
    p1.visible = true;

    // Reset Player 2
    p2.x = p2StartX;
    p2.y = p2StartY;
    p2.angle = 180;
    p2.armAngle = 0;
    p2.lives = 3;
    p2.aliveFlag = true;
    p2.isHit = false;
    p2.visible = true;

}

bool Game::LoadArena(const std::string &svgPath)
{
    std::vector<SvgCircle> circles;
    if(!SvgParser::ParseFile(svgPath, circles)) return false;

    obstacles.clear();

    int arenaIdx = -1;
    float maxR = -1.0f;

    for(size_t i=0; i<circles.size(); ++i){
        if(circles[i].r > maxR){
            maxR = circles[i].r;
            arenaIdx = (int)i;
        }
    }

    if(arenaIdx == -1) {
        printf("ERRO: Arena nao encontrada.\n"); //print no terminal
        return false;
    }

    float offsetX = circles[arenaIdx].cx;
    float offsetY = circles[arenaIdx].cy;
    arenaR = circles[arenaIdx].r;

    bool foundP1 = false;
    bool foundP2 = false;

    for(size_t i=0; i<circles.size(); ++i){
        if((int)i == arenaIdx) continue; 

        const auto &c = circles[i];
        float relX = c.cx - offsetX;
        float relY = -(c.cy - offsetY); 

        if(isColor(c.fill, "green")){
            p1.x = relX * PLAYER_SPREAD_FACTOR; 
            p1.y = relY * PLAYER_SPREAD_FACTOR;
            p1.headR = COMMON_RADIUS; 
            p1.angle = 0; p1.armAngle = 0; p1.lives = 3;
            // Inicializa campos de hit
            p1.isHit = false; p1.visible = true;
            p1StartX = p1.x; p1StartY = p1.y;
            foundP1 = true;
        }
        else if(isColor(c.fill, "red")){
            p2.x = relX * PLAYER_SPREAD_FACTOR;
            p2.y = relY * PLAYER_SPREAD_FACTOR;
            p2.headR = COMMON_RADIUS; 
            p2.angle = 180; p2.armAngle = 0; p2.lives = 3;
            // Inicializa campos de hit
            p2.isHit = false; p2.visible = true;
            p2StartX = p2.x; p2StartY = p2.y;
            foundP2 = true;
        }
        else {
            Obstacle o;
            o.x = (relX*2.25f) + 120.0f;
            o.y = relY*-0.5f - 100.f;
            o.r = COMMON_RADIUS*2.25f; 
            obstacles.push_back(o);
        }
    }
    
    return true;
}

void Game::InitGLWindow(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Trabalho 2D - Computacao Gráfica");
    glClearColor(1,1,1,1); 
    Resize(width,height);
    previousTime = glutGet(GLUT_ELAPSED_TIME);
}

void Game::Resize(int w,int h){
    width = w;
    height = h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float)w/(float)h;
    if(aspect >= 1.0f)
        glOrtho(-arenaR*aspect, arenaR*aspect, -arenaR, arenaR, -1,1);
    else
        glOrtho(-arenaR, arenaR, -arenaR/aspect, arenaR/aspect, -1,1);
        
    glMatrixMode(GL_MODELVIEW);
}

//Construção dos players
void Game::DrawPlayer(const Player &p, int id)
{
    // Se não estiver visível (piscando), não desenha nada
    if(!p.visible) return;

    glPushMatrix();
    
    glTranslatef(p.x, p.y, 0);
    glRotatef(p.angle, 0, 0, 1);
    glRotatef(90, 0, 0, 1); 

    if(id == 1) glColor3f(0, 1, 0); 
    else        glColor3f(1, 0, 0); 

    float r = p.headR; 
    
    float stride = r * 0.3f; 
    float lOffset = p.legForwardLeft ? stride : -stride;
    float rOffset = p.legForwardLeft ? -stride : stride;

    glColor3f(0.0f, 0.0f, 0.0f);
    float legW = r * 0.4f;
    float legL = r * 1.5f;
    float bodyW = r * 2.2f; 
    
    glBegin(GL_QUADS);
        glVertex2f(-bodyW/4 - legW/2, 0 + lOffset); 
        glVertex2f(-bodyW/4 + legW/2, 0 + lOffset);
        glVertex2f(-bodyW/4 + legW/2, -legL + lOffset);
        glVertex2f(-bodyW/4 - legW/2, -legL + lOffset);
    glEnd();

    glBegin(GL_QUADS);
        glVertex2f(bodyW/4 - legW/2, 0 + rOffset);
        glVertex2f(bodyW/4 + legW/2, 0 + rOffset);
        glVertex2f(bodyW/4 + legW/2, -legL + rOffset);
        glVertex2f(bodyW/4 - legW/2, -legL + rOffset);
    glEnd();

    if(id == 1) glColor3f(0, 1, 0); 
    else        glColor3f(1, 0, 0);

    glPushMatrix();
        glScalef(1.5f, 0.6f, 1.0f); 
        DrawCircle(0, 0, r); 
    glPopMatrix();

    float armL = r * 2.0f;
    float armW = r * 0.45f;
    
    glPushMatrix();
        glTranslatef(-bodyW/2, 0, 0); 
        glRotatef(p.armAngle, 0, 0, 1);
        glRotatef(180, 0, 0, 1); 
        
        glBegin(GL_QUADS);
            glVertex2f(-armW/2, 0);
            glVertex2f(armW/2, 0);
            glVertex2f(armW/2, armL);
            glVertex2f(-armW/2, armL);
        glEnd();
    glPopMatrix();

    DrawCircle(0, 0, r);
    
    glPopMatrix();
}

void Game::DrawText(const char *text, float x, float y) {
    glRasterPos2f(x, y);
    const char *c;
    for (c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void Game::Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(0,0,1);
    DrawCircle(0, 0, arenaR); 
    
    DrawObstacles();
    DrawPlayer(p1, 1); 
    DrawPlayer(p2, 2); 

    glColor3f(1, 0.5f, 0);
    for(auto &t: projectiles)
        if(t.alive) DrawCircle(t.x, t.y, PROJECTILE_RADIUS);

    // Placar e Interface
    glColor3f(0.0f, 0.0f, 0.0f); // Preto
    char buffer[100];
    
    // Desenha placar
    sprintf(buffer, "Verde: %d  |  Vermelho: %d", p1.lives, p2.lives);
    DrawText(buffer, -80, arenaR - 30);

    // Mensagem de Game Over
    if(p1.lives <= 0 || p2.lives <= 0) {
        if(p1.lives <= 0) sprintf(buffer, "VITORIA DO VERMELHO!");
        else              sprintf(buffer, "VITORIA DO VERDE!");
        
        DrawText(buffer, -90, 130);
        sprintf(buffer, "Pressione 'R' para Reiniciar");
        DrawText(buffer, -110, 100);
    }

    glutSwapBuffers();
}

void Game::DrawCircle(float cx,float cy,float r){
    glBegin(GL_POLYGON);
    for(int i=0;i<32;i++){
        float a = 2*M_PI*i/32;
        glVertex2f(cx+cos(a)*r, cy+sin(a)*r);
    }
    glEnd();
}

void Game::DrawObstacles()
{
    glColor3f(0, 0, 0); 
    for(const Obstacle &o : obstacles){
        DrawCircle(o.x, o.y, o.r);
    }
}

// Funções das Colisões
bool Game::CircleCollision(float x1,float y1,float r1, float x2,float y2,float r2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distSq = dx*dx + dy*dy;
    float sumR = r1 + r2;
    return distSq <= (sumR * sumR);
}

bool Game::PlayerCollidesArena(const Player &p, float nx, float ny) {
    float distSq = nx*nx + ny*ny;
    float maxDist = arenaR - p.headR;
    return distSq > (maxDist * maxDist);
}

bool Game::PlayerCollidesObstacles(const Player &p, float nx, float ny) {
    for(const auto &o : obstacles) {
        if(CircleCollision(nx, ny, p.headR, o.x, o.y, o.r)) {
            return true;
        }
    }
    return false;
}

bool Game::PlayersOverlap(float nx1, float ny1, float r1, float nx2, float ny2, float r2) {
    return CircleCollision(nx1, ny1, r1, nx2, ny2, r2);
}

// Retorna TRUE se atingiu um JOGADOR
bool Game::CheckProjectileCollisions(Projectile &pr) {
    if(!pr.alive) return false;

    // Obstáculos (Tiro some)
    for(const auto &o : obstacles) {
        if(CircleCollision(pr.x, pr.y, PROJECTILE_RADIUS, o.x, o.y, o.r)) {
            pr.alive = false;
            return false;
        }
    }

    Player *enemy = nullptr;
    if(pr.owner == 1) enemy = &p2;
    else              enemy = &p1;

    // Inimigo
    if(CircleCollision(pr.x, pr.y, PROJECTILE_RADIUS, enemy->x, enemy->y, enemy->headR)) {
        pr.alive = false;
        
        if (!enemy->isHit && enemy->lives > 0) {
            enemy->lives--;
            if (enemy->lives > 0) {
                enemy->isHit = true;
                enemy->hitTimer = 0;
                enemy->blinkTimer = 0;
                enemy->visible = false; 
                return true; // AVISO: Acertou jogador!
            }
        }
    }
    return false;
}

void Game::MouseMove(int x,int y){
    if(p1.lives <= 0 || p2.lives <= 0 || p1.isHit || p2.isHit) return;

    float centerX = width / 2.0f;
    float dx = x - centerX;
    float sensitivity = 45.0f / 150.0f; 
    
    float angle = dx * sensitivity;

    if(angle > 45.0f) angle = 45.0f;
    if(angle < -45.0f) angle = -45.0f;

    p1.armAngle = angle;
}


void Game::MouseClick(int btn,int state,int,int){
    if(p1.lives <= 0 || p2.lives <= 0 || p1.isHit || p2.isHit) return;

    //tiro player verde
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        float r = p1.headR;
        float bodyW = r * 2.2f;
        float armL = r * 2.0f;
        
        float totalAngRad = (p1.angle + p1.armAngle) * M_PI / 180.0f;
        float shoulderAngRad = (p1.angle + 270) * M_PI / 180.0f;
        float shoulderX = (bodyW / 2.0f) * cos(shoulderAngRad);
        float shoulderY = (bodyW / 2.0f) * sin(shoulderAngRad);
        float gunX = armL * cos(totalAngRad);
        float gunY = armL * sin(totalAngRad);

        Projectile t;
        t.x = p1.x + shoulderX + gunX;
        t.y = p1.y + shoulderY + gunY;
        t.dx = cos(totalAngRad);
        t.dy = sin(totalAngRad);
        t.speed = 300; 
        t.alive = true; t.owner = 1;
        projectiles.push_back(t);
    }
}

void Game::KeyDown(unsigned char key){
    keyState[key] = true;
    
    if(p1.lives <= 0 || p2.lives <= 0) {
        if(key == 'r' || key == 'R') Reset();
        return;
    }
    
    if (p1.isHit || p2.isHit) {
        return; 
    }

    //tiro player vermelho
    if(key == '5'){ 
        float r = p2.headR;
        float bodyW = r * 2.2f;
        float armL = r * 2.0f;

        float totalAngRad = (p2.angle + p2.armAngle) * M_PI / 180.0f;
        float shoulderAngRad = (p2.angle + 270) * M_PI / 180.0f;
        float shoulderX = (bodyW / 2.0f) * cos(shoulderAngRad);
        float shoulderY = (bodyW / 2.0f) * sin(shoulderAngRad);
        float gunX = armL * cos(totalAngRad);
        float gunY = armL * sin(totalAngRad);

        Projectile t;
        t.x = p2.x + shoulderX + gunX;
        t.y = p2.y + shoulderY + gunY;
        t.dx = cos(totalAngRad);
        t.dy = sin(totalAngRad);
        t.speed = 300;
        t.alive = true; t.owner = 2;
        projectiles.push_back(t);
    }
    
    if(key == 'r' || key == 'R') {
        Reset();
    }
}

void Game::KeyUp(unsigned char key){
    keyState[key] = false;
}

void Game::TryMove(Player &p, float dx, float dy, const Player &otherP) {
    float nextX = p.x + dx;
    float nextY = p.y + dy;

    if(PlayerCollidesArena(p, nextX, nextY)) return; 
    if(PlayerCollidesObstacles(p, nextX, nextY)) return;
    if(PlayersOverlap(nextX, nextY, p.headR, otherP.x, otherP.y, otherP.headR)) return;

    p.x = nextX;
    p.y = nextY;
}

void Game::Update(double dt){
    if(p1.lives <= 0 || p2.lives <= 0) return;

    // Se algum jogador estiver em estado de hit, atualiza timers
    if (p1.isHit || p2.isHit) {
        Player* victim = p1.isHit ? &p1 : &p2;
        
        victim->hitTimer += dt;
        victim->blinkTimer += dt;

        if (victim->blinkTimer >= 0.3f) {
            victim->visible = !victim->visible;
            victim->blinkTimer = 0;
        }

        if (victim->hitTimer >= 3.0f) {
            victim->isHit = false;
            victim->visible = true; 
        }
        return; 
    }

    float moveSpeed = 150 * dt;
    float rotSpeed  = 150 * dt;
    float armSpeed  = 100 * dt;

    //PLAYER VERDE (WASD)
    bool p1Moved = false;
    
    if(keyState['w']){ 
        float dx = cos(p1.angle*M_PI/180)*moveSpeed;
        float dy = sin(p1.angle*M_PI/180)*moveSpeed;
        TryMove(p1, dx, dy, p2); 
        p1Moved = true;
    }
    if(keyState['s']){ 
        float dx = -cos(p1.angle*M_PI/180)*moveSpeed;
        float dy = -sin(p1.angle*M_PI/180)*moveSpeed;
        TryMove(p1, dx, dy, p2); 
        p1Moved = true;
    }
    
    if(keyState['a']) p1.angle += rotSpeed; 
    if(keyState['d']) p1.angle -= rotSpeed; 
    
    if(p1Moved) {
        p1.legTimer += dt;
        if(p1.legTimer > 0.1f) { 
            p1.legForwardLeft = !p1.legForwardLeft;
            p1.legTimer = 0;
        }
    }

    //PLAYER VERMELHO (olkç)
    bool p2Moved = false;
    if(keyState['o'] || keyState['O']){ 
        float dx = cos(p2.angle*M_PI/180)*moveSpeed;
        float dy = sin(p2.angle*M_PI/180)*moveSpeed;
        TryMove(p2, dx, dy, p1); 
        p2Moved = true;
    }
    if(keyState['l'] || keyState['L']){ 
        float dx = -cos(p2.angle*M_PI/180)*moveSpeed;
        float dy = -sin(p2.angle*M_PI/180)*moveSpeed;
        TryMove(p2, dx, dy, p1); 
        p2Moved = true;
    }
    if(keyState['k'] || keyState['K']) p2.angle += rotSpeed; 
    //caso o layout do teclado não possua a tecla 'ç', aceitar ';' também
    if(keyState[CEDILLA_LOW] || keyState[CEDILLA_HIGH] || keyState[59] || keyState[58]) p2.angle -= rotSpeed; 
    
    if(keyState['4']) p2.armAngle += armSpeed; 
    if(keyState['6']) p2.armAngle -= armSpeed;
    
    if(p2.armAngle > 45) p2.armAngle = 45;
    if(p2.armAngle < -45) p2.armAngle = -45;

    if(p2Moved) {
        p2.legTimer += dt;
        if(p2.legTimer > 0.1f) {
            p2.legForwardLeft = !p2.legForwardLeft;
            p2.legTimer = 0;
        }
    }

    // Atualiza Tiros
    bool hitOccurred = false; 
    for(auto &t : projectiles){
        if(!t.alive) continue;
        t.x += t.dx * t.speed * dt;
        t.y += t.dy * t.speed * dt;
        
        float distSq = t.x*t.x + t.y*t.y;
        if(distSq > arenaR*arenaR) {
            t.alive = false;
        }
        
        if(CheckProjectileCollisions(t)) {
            hitOccurred = true;
        }
    }

    if(hitOccurred) {
        projectiles.clear();
    }
}

void Game::Idle(){
    int now = glutGet(GLUT_ELAPSED_TIME);
    double dt = (now - previousTime)/1000.0;
    previousTime = now;
    Update(dt);
    glutPostRedisplay();
}

void Game::ProcessInputForPlayer(Player&,int,double){}