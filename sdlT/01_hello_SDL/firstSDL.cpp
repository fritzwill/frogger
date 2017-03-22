//Using SDL and standard IO

#include<SDL2/SDL.h>
#include<stdio.h>

//screen dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main( int argc, char* args[]){
    //window we will render to
    SDL_Window* window = NULL;

    // surface contained by window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else{
        //Create window
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL){
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else{
            //get window surface
            screenSurface = SDL_GetWindowSurface(window);

            //fill the surface white
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

            //update the surface
            SDL_UpdateWindowSurface(window);

            //wait two seconds
            SDL_Delay(2000);
        }
    }
    //Destroy window
    SDL_DestroyWindow(window);

    //quit SDL subsystems
    SDL_Quit();

    return 0;
}
