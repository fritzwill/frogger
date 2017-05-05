// replica of the game frogger using SDL2 library
// Date: 5/1/2017
// Authors: Will Fritz, Tommy Lynch

// Using SDL, SDL_image, standard math, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <vector>

// help with directions of objects
enum Direction{
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
void addEnemies();
void loadObjects(bool);
void gameOver();

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

// main function
int main(int argc, char*args[]){
    loadObjects(true);
    RunGame();
}

// funciton to load all the textures and set initial values of their locations
void loadObjects(bool firstTime){
    backgroundPos.x = 0;
    backgroundPos.y = 0;
    backgroundPos.w = windowRect.w;
    backgroundPos.h = windowRect.h;
    
    if (firstTime){
        // check for failed initialization
        if( !InitEverything()) return;
    }
    // Load textures
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

}

// funciton to run the actual game
void RunGame(){
    bool loop = true; // used to run game loop
    bool paused = false; // used to handle if player pauses
    bool onLog = false; // used to keep track if player is on log
    Log *currLog; // what log the player is on
    
    while(loop){
        SDL_Event event;
        
        // handle if player is on log (move with log)
        if (onLog){
            switch(currLog->dir){
                case(Right):
                    playerPos.x += currLog->speed;
                    break;
                case(Left):
                    playerPos.x -= currLog->speed;
                    break;
            }
        }
        
        // handle user inputs  
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
                    // implement pause 
                    case SDLK_p:
                        SDL_Event pauseEvent;
                        paused = true;
                        while(paused){
                            while(SDL_PollEvent(&pauseEvent)){
                                if (pauseEvent.type == SDL_QUIT){
                                    loop = false;
                                    paused = false;
                                    continue;
                                }
                                else if(pauseEvent.type == SDL_KEYDOWN){
                                    if (pauseEvent.key.keysym.sym == SDLK_p){
                                        paused = false;
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        
        // move objects
        MoveEnemies();
        MoveLogs();

        // Check collisions against enemies
        if(CheckEnemyCollisions()){
            gameOver();  
            loop = false;  
            continue;      
        }

        // Check collisions against logs
        if (CheckLogCollisions()){
            onLog = true;
            currLog = getLog(); // getting log player is on
        }
        else{
            onLog = false;
            currLog = NULL;
        }
        
        // handle if player is in water and not on log
        if (!onLog && playerPos.y < 224 && playerPos.y > 45) { 
            gameOver();
            loop = false;
            continue;
        }
        
        // check if player is off screen
        if(playerPos.y > windowRect.h)
            playerPos.y = windowRect.h - 20;
        else if(playerPos.x < 0)
            playerPos.x = 0;
        else if(playerPos.x > windowRect.w)
            playerPos.x = windowRect.w - playerPos.w;


        // check collision against top bar (win the level)
        // since top bar covers the entire width, we only need to check y value
        // topBar.y refers to the top of the top bar, so topBar.y + topBar.h
        
        if(playerPos.y < (topBar.y + topBar.h)){
            ResetPlayerPos();

            // increase speed of new objects
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
    return;
}

// loads png texture give string of the png's path
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

// renderes all the objects to the screen so the game can run (called every loop iteration)
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

// false if something does not initialize correctly
bool InitEverything(){
    if(!InitSDL()) return false;

    if(!CreateWindow()) return false;
    if(!CreateRenderer()) return false;
    SetupRenderer();
    return true;
}

// helper to initialize SDL (false if can't initialize)
bool InitSDL(){
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1){
        std::cout << "Failed to initialize SDL : " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

// helper to create the game window (false if can't create)
bool CreateWindow(){
    window = SDL_CreateWindow("Server", windowRect.x, windowRect.y, windowRect.w, windowRect.h, 0);

    if(window == nullptr){
        std::cout << "Failed to create window: " << SDL_GetError();
        return false;
    }
    return true;
}

// helper to create renderer (false if can't create)
bool CreateRenderer(){
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == nullptr){
        std::cout << "Failed to create renderer : " << SDL_GetError();
        return false;
    }
    return true;
}

// helper to finilize renderer info
void SetupRenderer(){
    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize(renderer, windowRect.w, windowRect.h);

    // set color of renderer to red
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

// moves eveyr log on the screen according to their direction and speed
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

// moves every enemy according to their direction and speed
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

// checks for a general collision given two objects (all objects are rectangles)
// true if collision, false otherwise
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

// speciifcally checks if log opbjects collide with player
// true if they do, false otherwise
bool CheckLogCollisions(){
    for(const auto &p : logs){
        if(CheckCollision(p.pos, playerPos))
            return true;
    }

    return false;

}

// returns a pointer to the log that player collides with
Log * getLog(){
    for(auto &p : logs){
        if(CheckCollision(p.pos, playerPos))
            return &p;
    }
    return NULL;

}

// spcifically checks if trucks collide with player
// true if they collide, false otherwise
bool CheckEnemyCollisions(){
    for(const auto &p : enemies){
        if(CheckCollision(p.pos, playerPos))
            return true;
    }

    return false;

}

// Adds 3 enemies to a spciifc row
// row is determined by value of lastEnemyPos variable
void AddEnemy(){
    int speed = rand() % 3 + 1;
    // used to make random between left and right direciton
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

// Adds 1 long log and 1 short log to specific row
// row is determined by values of lastEnemyPos variable
// takes if direction to make sure they move in opposite directions when called in other functions
void AddLog(Direction dir){
    int speed = rand() % 3 + 1; // rand spped for entire row
    logs.push_back(Log({rand() % 100, lastEnemyPos, 40, 20}, speed, dir));
    logs.push_back(Log({rand() % 100 + 175, lastEnemyPos, 20, 20}, speed, dir));
    lastEnemyPos += 25; // so the next set of logs is on the next row
}

// adds enough objects ot fill the screen
void addEnemies(){
    // alternate left and right direction so player can always cross
    AddLog(Right);
    AddLog(Left);
    AddLog(Right);
    AddLog(Left);
    AddLog(Right);
    AddLog(Left);
    AddLog(Right);
    lastEnemyPos+=50; // skip green grass in middle
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
    AddEnemy();
}

// puts the player on bottom of map
void ResetPlayerPos(){
    playerPos.x = (windowRect.w /2) - (playerPos.w /2);
    playerPos.y = windowRect.h - bottomBar.h;
}

// displays game Over screen with player options
// happens when player dies
void gameOver(){

    // render the game over screen
    bool dead = true;
    while(dead){
        SDL_RenderClear(renderer);
        SDL_Texture * curr = LoadTexture("img/gameOver.png");
        SDL_RenderCopy(renderer, curr, NULL, &backgroundPos);
        SDL_RenderPresent(renderer);
        SDL_Event event;
        
        // take in user input
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT)
                return;
            else if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    /* implement restart */
                    case SDLK_r:
                        logs.clear();
                        enemies.clear();
                        lastEnemyPos = 50;
                        dead = false;
                        loadObjects(false);
                        RunGame();
                        break;
                    case SDLK_q:
                        dead = false;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
