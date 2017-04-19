// Tommy Lynch and Will Fritz

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

// PROTOTYPES
bool InitEverything();
bool InitSDL();
bool CreateWindow();
bool CreateRenderer();
void SetupRenderer();
void Render();
void RunGame();
void ResetPlayerPos();
SDL_Texture * LoadTexture(const std::string &str);

// CONSTANTS
SDL_Rect windowRect = {900, 300, 300, 400};
int movementFactor = 15;

// SDL VARIABLES
SDL_Window * window;
SDL_Renderer* renderer;

SDL_Rect playerPos;
SDL_Rect backgroundPos;

SDL_Texture* playerTexture;
SDL_Texture* backgroundTexture;


int main(int argc, char*args[]){
    backgroundPos.x = 0;
    backgroundPos.y = 0;
    backgroundPos.w = windowRect.w;
    backgroundPos.h = windowRect.h;

    if( !InitEverything()) return -1;

    playerTexture       = LoadTexture("player.png");
    backgroundTexture   = LoadTexture("background.bmp");

    // Initialize our player
    playerPos.w = 20;
    playerPos.h = 20;
    ResetPlayerPos();

    RunGame();
}

void RunGame(){
    bool loop = true;

    while(loop){
        SDL_Event event;
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
                    default:
                        break;
                }
            }
        }
        
        if(playerPos.y < 0)
            ResetPlayerPos();

        Render();

        SDL_Delay(15);
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

void ResetPlayerPos(){
    playerPos.x = (windowRect.w /2) - (playerPos.w /2);
    playerPos.y = windowRect.h - playerPos.w;
}


