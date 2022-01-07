#include "common.h"
#include "SDL2/SDL.h"
#include "db.h"

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define DISK_FILE_SIZE  (1024 * 1024 * 5) 
const char *disk_file = "temp";

int disk_initialize (void)
{
    if (access(disk_file, F_OK))
    {
        LOGI("Create file %s\n", disk_file);
        creat(disk_file, 0666);
    }

	return 0;
}

int db_read(uint32_t address, uint8_t *buff, int cnt)
{
    if (!access(disk_file, F_OK))
    {
        int fd = open(disk_file, O_RDWR);
        if (fd)
        {
            lseek(fd, address, SEEK_SET);
            int len = read(fd, buff, cnt);
            LOGI("Read address:%#x, cnt:%d\n", address, cnt);
            LOGHEX("Read data", buff, cnt);
            close(fd);
            return len;
        }
    }

	return 0;
}

int db_write(uint32_t address, uint8_t *buff, int cnt)
{
    if (!access(disk_file, F_OK))
    {
        int fd = open(disk_file, O_RDWR);
        if (fd)
        {
            lseek(fd, address, SEEK_SET);
            int len = write(fd, buff, cnt);
            LOGI("Write address:%#x, cnt:%d\n", address, cnt);
            LOGHEX("Write data", buff, cnt);
            close(fd);
            return len;
        }
    }
    
	return 0;
}

static void __attribute__((constructor)) facedb_test(void)
{
#if 1
    // Use file to test facedb
    disk_initialize();
#endif

#if 1
    LOGI("Facedb test!\n");

    db_err_t err = MF_OK;
    err = db_choose(DB_TYPE_FACE);
    if (MF_OK != err) {LOGE("Error %d\n", err); exit(-1);}
    
    err = db.init();
    if (MF_OK != err) {LOGE("Error %d\n", err); exit(-1);}

    LOGI("DB Insert\n");
    uint32_t index = 2047;
    uint8_t ftr[DB_FTR_SIZE];
    memset(ftr, 0x66, sizeof(ftr));
    err = db.insert(db_id2uid(index), &ftr);
    if (MF_OK != err) {LOGW("Can't insert %d, err=%d\n", index, err); }
    LOGI("Total face num: %d\n", db.num());

    LOGI("DB Select\n");
    uint8_t select_ftr[DB_FTR_SIZE];
    db_item_t select_item = {0};
    err = db.select(db_id2uid(index), &select_ftr);
    if (MF_OK != err) {LOGW("Can't select %d, err=%d\n", index, err); }
    if (memcmp(select_ftr, ftr, sizeof(ftr)))
    {
        LOGE("Select error, bad value!\n");
        LOGHEX("Insert ftr", ftr, sizeof(ftr));
        LOGHEX("Select ftr", select_ftr, sizeof(select_ftr));
    }

    LOGI("DB Update\n");
    uint8_t update_ftr[DB_FTR_SIZE];
    memset(update_ftr, 0x44, sizeof(update_ftr));
    err = db.update(db_id2uid(index), update_ftr);
    if (MF_OK != err) {LOGW("Can't update %d, err=%d\n", index, err); }
    err = db.select(db_id2uid(index), &select_ftr);
    if (MF_OK != err) {LOGW("Can't select %d, err=%d\n", index, err); }
    if (memcmp(select_ftr, update_ftr, DB_FTR_SIZE))
    {
        LOGE("Select error, bad value!\n");
        LOGHEX("Update ftr", update_ftr, DB_FTR_SIZE);
        LOGHEX("Select ftr", select_ftr, sizeof(select_ftr));
    }

    LOGI("DB Iterate\n");
    err = db.iterate_init();
    if (MF_OK != err) {LOGW("Can't init iterate %d, err=%d\n", index, err); }
    uint8_t iterate_uid[DB_UID_SIZE];
    uint8_t iterate_ftr[DB_FTR_SIZE];
    while (MF_CONTINUE == db.iterate(iterate_uid, iterate_ftr))
    {
        LOGI("index:%d\n", db_uid2id(iterate_uid));
        LOGHEX("uid", iterate_uid, DB_UID_SIZE);
        LOGHEX("ftr", iterate_ftr, DB_FTR_SIZE);
    }

    LOGI("DB Delete\n");
    err = db.delete(db_id2uid(index));
    if (MF_OK != err) {LOGW("Can't Delete %d, err=%d\n", index, err); }
    LOGI("Total face num: %d\n", db.num());

    err = db.deinit();
    if (MF_OK != err) {LOGE("Error %d\n", err); exit(-1);}

#endif 
}

static void __attribute__((destructor)) exit_handler(void)
{
    LOGI("Exit!\n");
}

int main(void)
{
    LOGI("Hello world!\n");
    
    return 0;
}