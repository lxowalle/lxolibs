#include "common.h"
#include "SDL2/SDL.h"

int main(void)
{
    LOGI("Hello world!\n");
    

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *win = NULL;
    win = SDL_CreateWindow("Hello", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == NULL)    {printf("Error:%s\n", SDL_GetError());  exit(-1);}

    SDL_Renderer *ren = NULL;
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (win == NULL)    {printf("Error:%s\n", SDL_GetError());  exit(-1);}

    SDL_Surface *bmp = NULL;
    bmp = SDL_LoadBMP("apps/app1/full.bmp");

    SDL_Texture *text = NULL;
    text = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);

    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, text, NULL, NULL);
    SDL_RenderPresent(ren);

    SDL_Delay(2000);

    SDL_DestroyTexture(text);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}