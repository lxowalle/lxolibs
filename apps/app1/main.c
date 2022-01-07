#include "common.h"
#include "SDL2/SDL.h"

static void __attribute__((constructor)) sdl2_show_bmp(void)
{
    LOGI("SDL2 show bmp!\n");
#if 0
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *win = NULL;
    win = SDL_CreateWindow("Hello", 100, 100, 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (win == NULL)    {printf("Error:%s\n", SDL_GetError());  exit(-1);}

    SDL_Renderer *ren = NULL;
    ren = SDL_CreateRenderer(win, -1, 0);
    if (win == NULL)    {printf("Error:%s\n", SDL_GetError());  exit(-1);}

    SDL_Surface *bmp = NULL;
    SDL_Texture *tet = NULL;
    bmp = SDL_LoadBMP("apps/app1/full.bmp");
    tet = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);

    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tet, NULL, NULL);
    SDL_RenderPresent(ren);

    SDL_Delay(2000);

    SDL_DestroyTexture(tet);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();
#endif
}

static void __attribute__((constructor)) sdl2_show_video(void)
{
#if 0
    LOGI("SDL2 show video!\n");

    const int bpp = 12;
    int screen_w = 1000, screen_h = 1000;
    const int pixel_w = 600, pixel_h = 750;
    uint8_t buff[pixel_w * pixel_h * bpp / 8];

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *win = NULL;
    win = SDL_CreateWindow("Hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (win == NULL)    {printf("Error:%s\n", SDL_GetError());  exit(-1);}

    SDL_Renderer *ren = NULL;
    ren = SDL_CreateRenderer(win, -1, 0);
    if (win == NULL)    {printf("Error:%s\n", SDL_GetError());  exit(-1);}

    uint32_t pixformat = SDL_PIXELFORMAT_IYUV;
    SDL_Texture *tet = NULL;
    tet = SDL_CreateTexture(ren, pixformat, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

    FILE *fp = NULL;
    fp = fopen("assets/full.yuv", "rb+");
    if (fp == NULL) {perror("Can not open file"); exit(-1);}

    SDL_Rect sdlRect;
    int x = 0;
    while (1)
    {
        fseek(fp, 0, SEEK_SET);
        fread(buff, 1, pixel_h * pixel_w * bpp / 8, fp);
        SDL_UpdateTexture(tet, NULL, buff, pixel_w);

        x ++;
        if (x > screen_w) x = 0;
        sdlRect.x = x;
        sdlRect.y = 0;
        sdlRect.w = pixel_w;
        sdlRect.h = pixel_h;

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tet, NULL, &sdlRect);
        SDL_RenderPresent(ren);

        SDL_Delay(10);
    }

    fclose(fp);
    SDL_DestroyTexture(tet);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();
#endif
}

int main(void)
{
    LOGI("Hello world!\n");
    
    return 0;
}