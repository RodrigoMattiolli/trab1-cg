#include "game.h"
#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <iostream>
#include <algorithm> // std::shuffle
#include <vector>
#include <random>    // std::default_random_engine, std::mt19937
#include <cstdio>

#define CEDILLA 186
extern Game *g_game;

// CONSTANTES DE AJUSTE VISUAL
static const float FIXED_HEAD_R = 15.0f;
static const float MAX_OBSTACLE_R = 50.0f;

Game::Game(){
    g_game = this;
    width = height = 500;

    // Valores padrão
    arenaR = 250;
    p1 = { -80, 0,   0, FIXED_HEAD_R, 0, 3, true, 0, true };
    p2 = {  80, 0, 180, FIXED_HEAD_R, 0, 3, true, 0, true };

    previousTime = glutGet(GLUT_ELAPSED_TIME);

    for(int i=0;i<256;i++)
        keyState[i] = false;
}

// Helper auxiliar de cor
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
}

bool Game::LoadArena(const std::string &svgPath)
{
    std::vector<SvgCircle> circles;
    if(!SvgParser::ParseFile(svgPath, circles)) return false;

    obstacles.clear();
    
    // Lista temporária para candidatos a obstáculo
    std::vector<Obstacle> potentialObstacles;

    // 1. Achar a Arena (Maior círculo)
    int arenaIdx = -1;
    float maxR = -1.0f;

    for(size_t i=0; i<circles.size(); ++i){
        if(circles[i].r > maxR){
            maxR = circles[i].r;
            arenaIdx = (int)i;
        }
    }

    if(arenaIdx == -1) {
        printf("ERRO: Arena nao encontrada.\n");
        return false;
    }

    float offsetX = circles[arenaIdx].cx;
    float offsetY = circles[arenaIdx].cy;
    arenaR = circles[arenaIdx].r;

    // 2. Carregar elementos relativos ao centro da Arena
    bool foundP1 = false;
    bool foundP2 = false;

    for(size_t i=0; i<circles.size(); ++i){
        if((int)i == arenaIdx) continue;

        const auto &c = circles[i];
        float relX = c.cx - offsetX;
        float relY = -(c.cy - offsetY); 

        if(isColor(c.fill, "green")){
            p1.x = relX; p1.y = relY;
            p1.headR = FIXED_HEAD_R; 
            p1.angle = 0; p1.armAngle = 0; p1.lives = 3;
            foundP1 = true;
        }
        else if(isColor(c.fill, "red")){
            p2.x = relX; p2.y = relY;
            p2.headR = FIXED_HEAD_R;
            p2.angle = 180; p2.armAngle = 0; p2.lives = 3;
            foundP2 = true;
        }
        else {
            // É um obstáculo em potencial (geralmente preto)
            Obstacle o;
            o.x = relX;
            o.y = relY;
            o.r = std::min(c.r, MAX_OBSTACLE_R);
            potentialObstacles.push_back(o);
        }
    }

    // --- CORREÇÃO ALEATORIEDADE E WARNING ---
    
    // Inicializa gerador de números aleatórios moderno
    std::random_device rd;
    std::mt19937 g(rd());

    // 1. Embaralha a lista de obstáculos potenciais
    std::shuffle(potentialObstacles.begin(), potentialObstacles.end(), g);

    // 2. Define quantos obstáculos queremos (entre 1 e 5)
    // Usa distribuição uniforme para garantir probabilidade igual
    std::uniform_int_distribution<int> dist(1, 5);
    int qtdSorteada = dist(g);
    
    // 3. Verifica quantos existem no SVG
    int disponiveis = (int)potentialObstacles.size();
    
    // O número final é o menor entre o sorteado e o disponível
    int qtdFinal = std::min(disponiveis, qtdSorteada);

    // LOG DE DEBUG PARA VOCÊ ENTENDER O QUE ACONTECEU
    printf("DEBUG: Obstaculos no SVG: %d. Sorteado: %d. Carregando: %d.\n", 
           disponiveis, qtdSorteada, qtdFinal);

    // Adiciona os escolhidos
    for(int i = 0; i < qtdFinal; i++) {
        obstacles.push_back(potentialObstacles[i]);
    }

    if(!foundP1) printf("AVISO: Player 1 (Verde) nao achado.\n");
    if(!foundP2) printf("AVISO: Player 2 (Vermelho) nao achado.\n");

    return true;
}

void Game::InitGLWindow(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Trabalho CG");
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

// ==========================================
// DESENHO DO PLAYER (Corrigido para Figura 2)
// ==========================================
void Game::DrawPlayer(const Player &p, int id)
{
    glPushMatrix();
    
    // Posiciona e Rotaciona o Jogador
    glTranslatef(p.x, p.y, 0);
    glRotatef(p.angle, 0, 0, 1);

    // Define cor
    if(id == 1) glColor3f(0, 1, 0); 
    else        glColor3f(1, 0, 0); 

    float r = p.headR;
    
    // --- CAMADA 1: PERNAS (Fundo / Preto) ---
    // Figura 2 mostra retângulos pretos saindo por baixo
    glColor3f(0.0f, 0.0f, 0.0f); // Preto
    
    float legW = r * 0.4f;
    float legL = r * 1.5f;
    float bodyW = r * 2.2f; // Tronco largo (elipse)
    
    // Perna Esquerda
    glBegin(GL_QUADS);
        glVertex2f(-bodyW/4 - legW/2, 0); // Nasce dentro do corpo
        glVertex2f(-bodyW/4 + legW/2, 0);
        glVertex2f(-bodyW/4 + legW/2, -legL);
        glVertex2f(-bodyW/4 - legW/2, -legL);
    glEnd();

    // Perna Direita
    glBegin(GL_QUADS);
        glVertex2f(bodyW/4 - legW/2, 0);
        glVertex2f(bodyW/4 + legW/2, 0);
        glVertex2f(bodyW/4 + legW/2, -legL);
        glVertex2f(bodyW/4 - legW/2, -legL);
    glEnd();

    // --- CAMADA 2: TRONCO (Meio / Verde/Vermelho) ---
    // Figura 2 mostra "ombros" saindo para os lados da cabeça.
    // Vamos desenhar uma elipse horizontal para simular isso.
    if(id == 1) glColor3f(0, 1, 0); 
    else        glColor3f(1, 0, 0);

    glPushMatrix();
        // Achata o círculo para virar uma elipse horizontal (ombros)
        glScalef(1.5f, 0.6f, 1.0f); 
        DrawCircle(0, 0, r); 
    glPopMatrix();

    // --- CAMADA 3: BRAÇO (Meio / Verde/Vermelho) ---
    float armL = r * 2.0f;
    float armW = r * 0.45f;
    
    glPushMatrix();
        // Pivô do braço: Ligeiramente à direita do centro
        glTranslatef(r * 0.5f, 0, 0); 
        glRotatef(p.armAngle, 0, 0, 1);
        
        // Desenha retângulo do braço
        glBegin(GL_QUADS);
            glVertex2f(0, -armW/2);
            glVertex2f(armL, -armW/2);
            glVertex2f(armL,  armW/2);
            glVertex2f(0,  armW/2);
        glEnd();
    glPopMatrix();

    // --- CAMADA 4: CABEÇA (Topo / Verde/Vermelho) ---
    // Círculo perfeito no centro, cobrindo as junções
    DrawCircle(0, 0, r);

    // Opcional: Nariz ou detalhe para indicar frente? 
    // A Fig 2 tem um triângulo/bico ("pontas" nos ombros). 
    // A elipse já simula bem essas pontas.
    
    glPopMatrix();
}

void Game::Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Arena
    glColor3f(0,0,1);
    DrawCircle(0, 0, arenaR); 
    
    DrawObstacles();
    DrawPlayer(p1, 1); 
    DrawPlayer(p2, 2); 

    // Tiros
    glColor3f(1, 0.5f, 0);
    for(auto &t: projectiles)
        if(t.alive) DrawCircle(t.x, t.y, 3);

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

// --- INPUT E LÓGICA ---

void Game::MouseMove(int x,int y){
    float nx = (float)x/width;   
    float ny = (float)y/height;  
    ny = 1.0f - ny; 

    float aspect = (float)width/height;
    float wW = (aspect>=1)? arenaR*aspect : arenaR;
    float wH = (aspect>=1)? arenaR : arenaR/aspect;

    float mouseWorldX = -wW + nx * 2 * wW;
    float mouseWorldY = -wH + ny * 2 * wH;

    float dx = mouseWorldX - p1.x;
    float dy = mouseWorldY - p1.y;
    
    p1.angle = atan2(dy,dx)*180/M_PI;
}

void Game::MouseClick(int btn,int state,int,int){
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        Projectile t;
        t.x = p1.x; t.y = p1.y;
        float totalAng = (p1.angle + p1.armAngle) * M_PI/180;
        t.dx = cos(totalAng);
        t.dy = sin(totalAng);
        t.speed = 300; 
        t.alive = true; t.owner = 1;
        projectiles.push_back(t);
    }
}

void Game::KeyDown(unsigned char key){
    keyState[key] = true;
    
    if(key == '5'){ 
        Projectile t;
        t.x = p2.x; t.y = p2.y;
        float totalAng = (p2.angle + p2.armAngle) * M_PI/180;
        t.dx = cos(totalAng);
        t.dy = sin(totalAng);
        t.speed = 300;
        t.alive = true; t.owner = 2;
        projectiles.push_back(t);
    }
    if(key == 'r' || key == 'R') {
        projectiles.clear();
    }
}

void Game::KeyUp(unsigned char key){
    keyState[key] = false;
}

void Game::Update(double dt){
    float moveSpeed = 150 * dt;
    float rotSpeed  = 150 * dt;

    // --- PLAYER 1 (WASD) ---
    if(keyState['w']){ 
        p1.x += cos(p1.angle*M_PI/180)*moveSpeed;
        p1.y += sin(p1.angle*M_PI/180)*moveSpeed;
    }
    if(keyState['s']){ 
        p1.x -= cos(p1.angle*M_PI/180)*moveSpeed;
        p1.y -= sin(p1.angle*M_PI/180)*moveSpeed;
    }
    if(keyState['a']) p1.angle += rotSpeed; 
    if(keyState['d']) p1.angle -= rotSpeed; 

    // --- PLAYER 2 (o/l + k/ç) ---
    if(keyState['o']){ 
        p2.x += cos(p2.angle*M_PI/180)*moveSpeed;
        p2.y += sin(p2.angle*M_PI/180)*moveSpeed;
    }
    if(keyState['l']){ 
        p2.x -= cos(p2.angle*M_PI/180)*moveSpeed;
        p2.y -= sin(p2.angle*M_PI/180)*moveSpeed;
    }
    if(keyState['k']) p2.angle += rotSpeed; 
    if(keyState[CEDILLA]) p2.angle -= rotSpeed; 

    // Atualiza Tiros
    for(auto &t : projectiles){
        if(!t.alive) continue;
        t.x += t.dx * t.speed * dt;
        t.y += t.dy * t.speed * dt;
        if((t.x*t.x + t.y*t.y) > arenaR*arenaR) t.alive = false;
    }
}

void Game::Idle(){
    int now = glutGet(GLUT_ELAPSED_TIME);
    double dt = (now - previousTime)/1000.0;
    previousTime = now;
    Update(dt);
    glutPostRedisplay();
}

// Placeholders
void Game::DrawText(const char *,float,float){}
bool Game::CircleCollision(float,float,float,float,float,float){return false;}
bool Game::PlayerCollidesArena(const Player&,float,float){return false;}
bool Game::PlayerCollidesObstacles(const Player&,float,float){return false;}
bool Game::PlayersOverlap(float,float,float,float,float,float){return false;}
void Game::CheckProjectileCollisions(Projectile&){}
void Game::ProcessInputForPlayer(Player&,int,double){}