//Using SDL, SDL_image, standard math, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <vector>
enum /*class*/ Direction{
    Left,
    Right
};

// Objects in game
struct Log{
    Log(SDL_Rect pos_, int speed_, Direction dir_){
        pos = pos_;
        speed = speed_;
        dir = dir_;
    }
    SDL_Rect pos;
    int speed;
    Direction dir;
};

struct Enemy{
    Enemy(SDL_Rect pos_, int speed_, Direction dir_){
        pos = pos_;
        speed = speed_;
        dir = dir_;
    }
    SDL_Rect pos;
    int speed;
    Direction dir;
};

// PROTOTYPES
bool InitEverything();
bool InitSDL();
bool CreateWindow();
bool CreateRenderer();
void SetupRenderer();
SDL_Texture * LoadTexture(const std::string &str);
void Render();
void RunGame();
void AddEnemy();
void AddLog(Direction dir );
void MoveEnemies();
void MoveLogs();
void ResetPlayerPos();
bool CheckCollision( const SDL_Rect &rect1, const SDL_Rect &rect2);
bool CheckEnemyCollisions();
bool CheckLogCollisions();
Log * getLog();
int getDiff(SDL_Rect, SDL_Rect);
void addEnemies();

// Global Variables
SDL_Rect windowRect = {900, 200, 300, 500};

int movementFactor = 25;
int lastEnemyPos = 50;

SDL_Window * window;
SDL_Renderer* renderer;

SDL_Rect playerPos;
SDL_Rect topBar;
SDL_Rect bottomBar;
SDL_Rect backgroundPos;

SDL_Texture* enemyTexture;
SDL_Texture* logTexture;
SDL_Texture* playerTexture;
SDL_Texture* backgroundTexture;
SDL_Texture* barTexture;

std::vector<Enemy> enemies;
std::vector<Log> logs;

int main(int argc, char*args[]){

    backgroundPos.x = 0;
    backgroundPos.y = 0;
    backgroundPos.w = windowRect.w;
    backgroundPos.h = windowRect.h;

    if( !InitEverything()) return -1;

    enemyTexture        = LoadTexture("img/truck.png");
    logTexture          = LoadTexture("img/logLong.png");
    playerTexture       = LoadTexture("img/frog.png");
    backgroundTexture   = LoadTexture("img/background.bmp");
    barTexture          = LoadTexture("img/bar.bmp");
    srand(time(NULL));
    
    // Adding moving objects
    addEnemies();
    
    // Init top and bottom bar
    topBar.x = 0;
    topBar.y = 0;
    topBar.w = windowRect.w;
    topBar.h = 20;

    bottomBar.x = 0;
    bottomBar.y = windowRect.h - 20;
    bottomBar.w = windowRect.w;
    bottomBar.h = 20;

    // Initialize our player
    playerPos.w = 20;
    playerPos.h = 15;
    ResetPlayerPos();

    RunGame();
}

void RunGame(){
    bool loop = true;
    int score = 0;
    int diff = 0;
    bool onLog = false;
    Log *currLog;
    while(loop){
        SDL_Event event;
        if (onLog){

            //diff = getDiff(playerPos, currLog->pos);
            switch(currLog->dir){
                case(Right):
                    playerPos.x += currLog->speed;
                    break;
                case(Left):
                    playerPos.x -= currLog->speed;
                    break;
            }
        }
            
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT)
                loop = false;
            else if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_RIGHT:
                        playerPos.x += movementFactor;
                        break;
                    case SDLK_LEFT:
                        playerPos.x -= movementFactor;
                        break;
                    case SDLK_DOWN:
                        playerPos.y += movementFactor;
                        break;
                    case SDLK_UP:
                        playerPos.y -= movementFactor;
                        break;
                    /* implement pause */
                    /*case SDLK_p:
                        while(SDL_PollEvent(&event){*/

                    default:
                        break;
                }
            }
        }
        MoveEnemies();
        MoveLogs();

        // Check collisions against enemies
        if(CheckEnemyCollisions()) 
            std::cout << "HAHAHA";            
            //ResetPlayerPos();

        // Check collisions against logs
        if (CheckLogCollisions()){
            onLog = true;
            currLog = getLog();
            std::cout << "in func" << std::endl;
            //diff = getDiff(playerPos, currLog->pos);
            
            
        }
        else{
            onLog = false;
            currLog = NULL;
        }
        
        std::cout << "passed func" << std::endl;
        if (!onLog && playerPos.y < 224 && playerPos.y > 45) { // not on log and above halfway
            ResetPlayerPos();
        }

        // check collision against bottom bar
        // since top bar covers the entire width, we only need to check y value
        // topBar.y refers to the top of the top bar, so topBar.y + topBar.h
        
        if(playerPos.y < (topBar.y + topBar.h)){
            ResetPlayerPos();
            score += 10;
            // increase speed of objects
            std::vector<int> logSpeeds;
            std::vector<int> truckSpeeds;
            for(auto &p : logs){
               logSpeeds.push_back(p.speed);
            }
            for(auto &p : enemies)
                truckSpeeds.push_back(p.speed); 
            logs.clear();
            enemies.clear();
            lastEnemyPos = 50;
            addEnemies();
            int count = 0; 
            for (auto &p : enemies){
                p.speed = truckSpeeds[count] * 1.2; 
                count++;
            }
            count = 0;
            for (auto &p : logs){
                p.speed = logSpeeds[count] * 1.2;
                count++;
            }
            
        }
        Render();

        // add a 16 msec delay so it funs at ~60fps
        SDL_Delay(16);
    }
}

SDL_Texture* LoadTexture(const std::string &str){
    // Load image as SDL_Surface
    SDL_Surface* surface = IMG_Load(str.c_str());
    
    // SDL Surface is just the raw pixels, so we convert it to a 
    // hardware-optimized texture so we can render it
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // dont need the original trexture, release memory
    SDL_FreeSurface(surface);
    return texture;
}

void Render(){
    // Clear the window and make it red
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundPos);
    SDL_RenderCopy(renderer, barTexture, NULL, &topBar);
    SDL_RenderCopy(renderer, barTexture, NULL, &bottomBar);
    for(const auto &p : enemies)
        SDL_RenderCopy(renderer, enemyTexture, NULL, &p.pos);
    for(const auto &p : logs)
        SDL_RenderCopy(renderer, logTexture, NULL, &p.pos);

    SDL_RenderCopy(renderer, playerTexture, NULL, &playerPos);
    // render the changes above
    SDL_RenderPresent(renderer);
}
bool InitEverything(){
    if(!InitSDL()) return false;
    if(!CreateWindow()) return false;
    if(!CreateRenderer()) return false;
    SetupRenderer();
    return true;
}

bool InitSDL(){
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1){
        std::cout << "Failed to initialize SDL : " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool CreateWindow(){
    window = SDL_CreateWindow("Server", windowRect.x, windowRect.y, windowRect.w, windowRect.h, 0);

    if(window == nullptr){
        std::cout << "Failed to create window: " << SDL_GetError();
        return false;
    }
    return true;
}

bool CreateRenderer(){
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == nullptr){
        std::cout << "Failed to create renderer : " << SDL_GetError();
        return false;
    }
    return true;
}

void SetupRenderer(){
    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize(renderer, windowRect.w, windowRect.h);

    // set color of renderer to red
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

void MoveLogs(){
    for(auto &p : logs){
        if(p.dir == Direction::Right){
            p.pos.x += p.speed;

            if(p.pos.x >= windowRect.w)
                p.pos.x = 0;
        }
        else{
            p.pos.x -= p.speed;

            if((p.pos.x + p.pos.w) <= 0)
                p.pos.x = windowRect.w -p.pos.w;
        }
    }
}
void MoveEnemies(){
    for(auto &p : enemies){
        if(p.dir == Direction::Right){
            p.pos.x += p.speed;

            if(p.pos.x >= windowRect.w)
                p.pos.x = 0;
        }
        else{
            p.pos.x -= p.speed;

            if((p.pos.x + p.pos.w) <= 0)
                p.pos.x = windowRect.w -p.pos.w;
        }
    }
}

bool CheckCollision(const SDL_Rect &rect1, const SDL_Rect &rect2){
    // find edges of rect1 and rect2

    int left1 = rect1.x;
    int right1 = rect1.x + rect1.w;
    int top1 = rect1.y;
    int bottom1 = rect1.y + rect1.h;

    int left2 = rect2.x;
    int right2 = rect2.x + rect2.w;
    int top2 = rect2.y;
    int bottom2 = rect2.y + rect2.h;

    // check edges
    if(left1 > right2) return false; // left 1 is right of right 2
    if(right1 < left2) return false;
    if(top1 > bottom2) return false;
    if(bottom1 < top2) return false;

    return true;
}

bool CheckLogCollisions(){
    for(const auto &p : logs){
        if(CheckCollision(p.pos, playerPos))
            return true;
    }

    return false;

}

Log * getLog(){
    for(auto &p : logs){
        if(CheckCollision(p.pos, playerPos))
            return &p;
    }
    return NULL;

}




bool CheckEnemyCollisions(){
    for(const auto &p : enemies){
        if(CheckCollision(p.pos, playerPos))
            return true;
    }

    return false;

}
int getDiff(SDL_Rect player, SDL_Rect log){
    return player.x - log.x;
}

void AddEnemy(){
    int speed = rand() % 3 + 1;
    if((rand() % 2 ) == 0){
        enemies.push_back(Enemy({rand() % 100, lastEnemyPos, 20, 20}, speed, Direction::Right));
        enemies.push_back(Enemy({rand() % 100 + 75, lastEnemyPos, 20, 20}, speed, Direction::Right));
        enemies.push_back(Enemy({rand() % 100 + 175, lastEnemyPos, 20, 20}, speed, Direction::Right));
    }
    else{
        enemies.push_back(Enemy({rand() % 100, lastEnemyPos, 20, 20}, speed, Direction::Left));
        enemies.push_back(Enemy({rand() % 100 + 75, lastEnemyPos, 20, 20}, speed, Direction::Left));
        enemies.push_back(Enemy({rand() % 100 + 175, lastEnemyPos, 20, 20}, speed, Direction::Left));
    }
    lastEnemyPos += 25; // so next set of enemies is on the next row
}

void AddLog(Direction dir){
    //if((rand() % 2) == 0){
        int speed = rand() % 3 + 1;
        logs.push_back(Log({rand() % 100, lastEnemyPos, 40, 20}, speed, dir));
        logs.push_back(Log({rand() % 100 + 175, lastEnemyPos, 20, 20}, speed, dir));
    /*}
    else{
        logs.push_back(Log({rand() % 100, lastEnemyPos, 40, 20}, 2, Direction::Left));
        logs.push_back(Log({rand() % 100 + 100, lastEnemyPos, 40, 20}, 2, Direction::Left));
        logs.push_back(Log({rand() % 100 + 200, lastEnemyPos, 20, 20}, 2, Direction::Left));
    }*/
    lastEnemyPos += 25; // so the next set of logs is on the next row
}
void addEnemies(){
    AddLog(Right);
    AddLog(Left);
    AddLog(Right);
    AddLog(Left);
    AddLog(Right);
    AddLog(Left);
    AddLog(Right);
    lastEnemyPos+=50;
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
}
void ResetPlayerPos(){
    playerPos.x = (windowRect.w /2) - (playerPos.w /2);
    playerPos.y = windowRect.h - bottomBar.h;
}


