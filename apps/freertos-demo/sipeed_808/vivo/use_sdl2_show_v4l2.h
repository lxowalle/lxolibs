#ifndef __USE_SDL2_SHOW_V4L2_H
#define __USE_SDL2_SHOW_V4L2_H

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct
{
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Texture *tet;
    SDL_Rect sdlRect;
}user_sdl_t;

user_sdl_t *get_sdl_handle(void);
int use_sdl2_show_v4l2_init(void);
int use_sdl2_show_v4l2_deinit(void);
int use_sdl2_show_v4l2_loop_test(void);

#endif /* __USE_SDL2_SHOW_V4L2_H */