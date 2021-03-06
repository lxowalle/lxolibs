#include "common.h"

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <SDL2/SDL.h>


void handle_sigint( int signal )
{
    LOGI("Get exit signal!\n");
    exit(0);
}

typedef struct
{
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Texture *tet;
    SDL_Rect sdlRect;
}user_sdl_t;
user_sdl_t user_sdl;

#define SCREEN_WIDTH    (320)
#define SCREEN_HEIGHT   (240)
#define PIC_WIDTH       (320)
#define PIC_HEIGHT      (240)

static int sdl_init(void)
{
    user_sdl_t *sdl = (user_sdl_t *)&user_sdl;

    SDL_Init(SDL_INIT_EVERYTHING);

    sdl->win = SDL_CreateWindow("User", 0, 0, 
                                SCREEN_WIDTH, SCREEN_HEIGHT, 
                                SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (sdl->win == NULL)    {LOGE("Error:%s\n", SDL_GetError());  return -1;}                    

    sdl->ren = SDL_CreateRenderer(sdl->win, -1, 0);
    if (sdl->ren == NULL)    {LOGE("Error:%s\n", SDL_GetError());  return -1;}

    sdl->tet = SDL_CreateTexture(sdl->ren, SDL_PIXELFORMAT_IYUV, 
                                SDL_TEXTUREACCESS_STREAMING, 
                                PIC_WIDTH, PIC_HEIGHT);
    if (sdl->tet == NULL)    {LOGE("Error:%s\n", SDL_GetError());  return -1;}

    return 0;
}

static int sdl_deinit(void)
{
    user_sdl_t *sdl = (user_sdl_t *)&user_sdl;
    if (sdl->tet)   SDL_DestroyTexture(sdl->tet);
    if (sdl->ren)   SDL_DestroyRenderer(sdl->ren);
    if (sdl->win)   SDL_DestroyWindow(sdl->win);
    SDL_Quit();

    return 0;
}

#define TRUE            (1)
#define FALSE           (0)

#define FILE_VIDEO      "/dev/video0"
#define IMAGE           "./assets/demo"

#define IMAGEWIDTH      320
#define IMAGEHEIGHT     240

#define FRAME_NUM       4

int fd;
struct v4l2_buffer buf;

struct buffer
{
    void * start;
    unsigned int length;
    long long int timestamp;
} *buffers;


int v4l2_init()
{
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_format fmt;
    struct v4l2_streamparm stream_para;

    //?????????????????????
    if ((fd = open(FILE_VIDEO, O_RDWR)) == -1) 
    {
        printf("Error opening V4L interface\n");
        return FALSE;
    }

    //??????????????????
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) 
    {
        printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);
        return FALSE;
    }
    else
    {
        printf("driver:\t\t%s\n",cap.driver);
        printf("card:\t\t%s\n",cap.card);
        printf("bus_info:\t%s\n",cap.bus_info);
        printf("version:\t%d\n",cap.version);
        printf("capabilities:\t%x\n",cap.capabilities);
        
        if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) 
        {
            printf("Device %s: supports capture.\n",FILE_VIDEO);
        }

        if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) 
        {
            printf("Device %s: supports streaming.\n",FILE_VIDEO);
        }
    }


    //???????????????????????????
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("Support format:\n");
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
    {
        printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
        fmtdesc.index++;
    }

    //??????????????????????????????
    struct v4l2_format fmt_test;
    fmt_test.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt_test.fmt.pix.pixelformat=V4L2_PIX_FMT_RGB32;
    if(ioctl(fd,VIDIOC_TRY_FMT,&fmt_test)==-1)
    {
        printf("not support format RGB32!\n");      
    }
    else
    {
        printf("support format RGB32\n");
    }

    fmt_test.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;
    if(ioctl(fd,VIDIOC_TRY_FMT,&fmt_test)==-1)
    {
        printf("not support format YUV422!\n");      
    }
    else
    {
        printf("support format YUV422\n");
    }

    //???????????????????????????
    printf("set fmt...\n");
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32; //jpg??????
    // fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//yuv??????

    fmt.fmt.pix.height = IMAGEHEIGHT;
    fmt.fmt.pix.width = IMAGEWIDTH;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    printf("fmt.type:\t\t%d\n",fmt.type);
    printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
    printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
    printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
    printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
        printf("Unable to set format\n");
        return FALSE;
    }

    printf("get fmt...\n"); 
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
    {
        printf("Unable to get format\n");
        return FALSE;
    }
    {
        printf("fmt.type:\t\t%d\n",fmt.type);
        printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
        printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
        printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
        printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
    }

    //??????????????????????????????????????????30????????????1?????????30??????
    memset(&stream_para, 0, sizeof(struct v4l2_streamparm));
    stream_para.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    stream_para.parm.capture.timeperframe.denominator = 30;
    stream_para.parm.capture.timeperframe.numerator = 1;

    if(ioctl(fd, VIDIOC_S_PARM, &stream_para) == -1)
    {
        printf("Unable to set frame rate\n");
        return FALSE;
    }
    if(ioctl(fd, VIDIOC_G_PARM, &stream_para) == -1)
    {
        printf("Unable to get frame rate\n");
        return FALSE;       
    }
    {
        printf("numerator:%d\ndenominator:%d\n",stream_para.parm.capture.timeperframe.numerator,stream_para.parm.capture.timeperframe.denominator);
    }
    return TRUE;
}


int v4l2_mem_ops()
{
    unsigned int n_buffers;
    struct v4l2_requestbuffers req;
    
    //???????????????
    req.count=FRAME_NUM;
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory=V4L2_MEMORY_MMAP;
    if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1)
    {
        printf("request for buffers error\n");
        return FALSE;
    }

    // ??????????????????????????????
    buffers = malloc(req.count*sizeof (*buffers));
    if (!buffers) 
    {
        printf ("out of memory!\n");
        return FALSE;
    }
    
    // ??????????????????
    for (n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++) 
    {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        //??????
        if (ioctl (fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            printf("query buffer error\n");
            return FALSE;
        }

        //??????
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[n_buffers].start == MAP_FAILED)
        {
            printf("buffer map error\n");
            return FALSE;
        }
    }
    return TRUE;    
}


#include "SDL2/SDL_image.h"
int v4l2_frame_process()
{
    unsigned int n_buffers;
    enum v4l2_buf_type type;
    char file_name[100];
    char index_str[10];
    long long int extra_time = 0;
    long long int cur_time = 0;
    long long int last_time = 0;

    //?????????????????????
    for (n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++)
    {
        buf.index = n_buffers;
        ioctl(fd, VIDIOC_QBUF, &buf);
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);
    

    //????????????????????????yuv??????????????????????????????
    int loop = 0;
    while(1)
    {
        for(n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++)
        {
            //??????
            buf.index = n_buffers;
            ioctl(fd, VIDIOC_DQBUF, &buf);

            //??????????????????????????????????????????????????????
            buffers[n_buffers].timestamp = buf.timestamp.tv_sec*1000000+buf.timestamp.tv_usec;
            cur_time = buffers[n_buffers].timestamp;
            extra_time = cur_time - last_time;
            last_time = cur_time;
            printf("time_deta:%lld ms\n\n",extra_time / 1000);
            printf("buf_len:%d\n",buffers[n_buffers].length);

#if 0
            /* ????????????????????????????????????????????????loop????????????????????????????????? */
            // printf("grab image data OK\n");
            // memset(file_name,0,sizeof(file_name));
            // memset(index_str,0,sizeof(index_str));
            // sprintf(index_str,"%d",loop*4+n_buffers);
            // strcpy(file_name,IMAGE);
            // strcat(file_name,index_str);
            // strcat(file_name,".jpg");
            // // strcat(file_name,".yuv");
            // FILE *fp2 = fopen(file_name, "wb");
            // if(!fp2)
            // {
            //     printf("open %s error\n",file_name);
            //     return(FALSE);
            // }
            // fwrite(buffers[n_buffers].start, IMAGEHEIGHT*IMAGEWIDTH*2,1,fp2);
            // fclose(fp2);
            // printf("save %s OK\n",file_name);
#else
            user_sdl_t *sdl = (user_sdl_t *)&user_sdl;
            SDL_RWops *dst = SDL_RWFromMem(buffers[n_buffers].start, buffers[n_buffers].length);
            if (!dst)
            {
                LOGE("SDL_RWFromFile failed\n");
            }
            SDL_Surface *sur = IMG_LoadJPG_RW(dst);
            if (!sur)
            {
                LOGE("IMG_LoadJPG_RW failed\n");
            }
            SDL_Texture *tet = SDL_CreateTextureFromSurface(sdl->ren, sur);
            SDL_FreeRW(dst);
            SDL_FreeSurface(sur);

            SDL_RenderClear(sdl->ren);
            SDL_RenderCopy(sdl->ren, tet, NULL, NULL);
            SDL_RenderPresent(sdl->ren);
#endif
            //????????????
            ioctl(fd, VIDIOC_QBUF, &buf);       
        }

        loop++;
    }
    return TRUE;    
}




int v4l2_release()
{
    unsigned int n_buffers;
    enum v4l2_buf_type type;

    //?????????
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);
    
    //??????????????????
    for(n_buffers=0;n_buffers<FRAME_NUM;n_buffers++)
    {
        munmap(buffers[n_buffers].start,buffers[n_buffers].length);
    }
    
    //???????????????????????????
    free(buffers);
    
    //????????????
    close(fd);
    return TRUE;
}




/*int v4l2_video_input_output()
{
    struct v4l2_input input;
    struct v4l2_standard standard;

    //???????????????????????????index,????????????index??????????????????????????????????????????????????????
    memset (&input,0,sizeof(input));
    if (-1 == ioctl (fd, VIDIOC_G_INPUT, &input.index)) {
        printf("VIDIOC_G_INPUT\n");
        return FALSE;
    }
    //??????????????????????????? input.index ??????????????????????????????
    if (-1 == ioctl (fd, VIDIOC_ENUMINPUT, &input)) {
        printf("VIDIOC_ENUM_INPUT \n");
        return FALSE;
    }
    printf("Current input %s supports:\n", input.name);


    //??????????????????????????? standard????????? standard.id ????????? input ??? input.std ????????????
    //bit flag??????????????????????????????????????? standard,????????????????????????????????? standard ????????????
    //???????????????????????????????????????????????? standard ??????

    memset(&standard,0,sizeof (standard));
    standard.index = 0;
    while(0 == ioctl(fd, VIDIOC_ENUMSTD, &standard)) {
        if (standard.id & input.std){
            printf ("%s\n", standard.name);
        }
        standard.index++;
    }
    // EINVAL indicates the end of the enumeration, which cannot be empty unless this device falls under the USB exception. 

    if (errno != EINVAL || standard.index == 0) {
        printf("VIDIOC_ENUMSTD\n");
        return FALSE;
    }

}*/


static void __attribute__((constructor)) sdl2_show_video(void)
{
    signal( SIGINT, handle_sigint );
#if 0
    LOGI("SDL2 show video!\n");

    int res = 0;
    const int bpp = 12;
    uint8_t buff[PIC_WIDTH * PIC_HEIGHT * bpp / 8];

    user_sdl_t *sdl = (user_sdl_t *)&user_sdl;

    res = sdl_init();
    if (res < 0)    {LOGE("Error:%s\n", SDL_GetError());  exit(0);}

    FILE *fp = NULL;
    fp = fopen("assets/demo44.jpg", "rb+");
    if (fp == NULL) {perror("Can not open file"); exit(-1);}

    SDL_Rect sdlRect;
    int x = 0;
    while (1)
    {
        fseek(fp, 0, SEEK_SET);
        fread(buff, 1, PIC_HEIGHT * PIC_WIDTH * bpp / 8, fp);
        SDL_UpdateTexture(sdl->tet, NULL, buff, PIC_WIDTH);

        x ++;
        if (x > SCREEN_WIDTH) x = 0;
        sdlRect.x = x;
        sdlRect.y = 0;
        sdlRect.w = PIC_WIDTH;
        sdlRect.h = PIC_HEIGHT;

        SDL_RenderClear(sdl->ren);
        SDL_RenderCopy(sdl->ren, sdl->tet, NULL, &sdlRect);
        SDL_RenderPresent(sdl->ren);

        SDL_Delay(10);
    }

    fclose(fp);

    res = sdl_deinit();
    if (res < 0)    {LOGE("Error:%s\n", SDL_GetError());  exit(0);}
#endif
}

__attribute__((constructor)) static void _start_handler(void)
{
    int res = sdl_init();
    if (res < 0)    {LOGE("Error:%s\n", SDL_GetError());  exit(0);}

    LOGI("start handler!\n");

    printf("begin....\n");
    sleep(1);

    v4l2_init();
    printf("init....\n");
    sleep(1);

    v4l2_mem_ops();
    printf("malloc....\n");
    sleep(1);

#if 1
    v4l2_frame_process();
    printf("process....\n");
    sleep(1);
#endif

    v4l2_release();
    printf("release\n");
    sleep(1);

    res = sdl_deinit();
    if (res < 0)    {LOGE("Error:%s\n", SDL_GetError());  exit(0);}
}

__attribute__((destructor)) static void _exit_handler(void)
{
    LOGI("exit handler!\n");
}

int main(int argc, char const *argv[])
{
    LOGI("Hello world!\n");
    

    return 0;
}