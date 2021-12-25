#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "ff.h"

#ifndef PRINF_HEX_ARR
#define PRINF_HEX_ARR(str,buf,len)\
do{\
    char *buff = (char *)buf;\
    printf("\e[32m[%s](%ld):\e[0m", str, len);\
    for (int i = 0;i < len; ++i)\
    {\
        printf("0x%.2X ", buff[i] & 0xff);\
    }\
    printf("\r\n");\
} while (0);
#endif

int main(void)
{
    printf("libfatfs test\n");

#if 0
    const char *disk_file = "fatfs_vdisk";
    int sector = 0, count = 1;
    if (!access(disk_file, F_OK))
    {
        int fd = open(disk_file, O_RDWR);
        if (fd)
        {
            uint8_t buff0[128] = "1234566";
            lseek(fd, FF_MIN_SS * sector, SEEK_SET);
            int len0 = write(fd, buff0, sizeof(buff0));

            uint8_t buff[128] = "";
            lseek(fd, FF_MIN_SS * sector, SEEK_SET);
            int len = read(fd, buff, sizeof(buff));

            printf("\e[32m[%s](%d):\e[0m", "read", len);
            for (int i = 0;i < len; ++i)
            {
                printf("0x%.2X ", buff[i] & 0xff);
            }
            printf("\r\n");

            close(fd);
        }
    }

    return 0;
#endif
    FATFS fs;
    FIL fil;
    FRESULT res;

    BYTE work[FF_MAX_SS];
    LBA_t plist[] = {50, 50, 0};
    res = f_fdisk(0, plist, work);
    printf("f_fdisk res:%d\n", res);
    PRINF_HEX_ARR("work", work, sizeof(work));
    res = f_mkfs("0:", 0, work, sizeof(work));
    printf("f_mkfs 0: res:%d\n", res);

    #define ROOT_PATH1   "/1"
    res = f_mkfs(ROOT_PATH1, 0, work, sizeof(work));
    printf("f_mkfs / res:%d\n", res);

    res = f_mount(&fs, "0:", 1);
    printf("f_mount 0: res:%d\n", res);

    res = f_mount(&fs, ROOT_PATH1, 1);
    printf("f_mount /1 res:%d\n", res);

#if 1
    {
        FIL fp;
        res = f_open(&fp, "0:a", FA_CREATE_NEW | FA_WRITE | FA_READ);
        if (!fp.obj.fs)
            printf("fp obj fs is NULL\n");
        printf("f_open res:%d\n", res);

        int bw;
        res = f_write(&fp, "123456789", strlen("123456789"), &bw);
        printf("write data(%d)  f_write res:%d\n", bw, res);

        res = f_sync(&fp);
        printf("f_sync res:%d\n", res);

        uint8_t read_buff[100];
        int br;
        f_lseek(&fp, 0);

        res = f_read(&fp, read_buff, sizeof(read_buff), &br);
        printf("read 0:a data(%d):%s  f_read res:%d\n", br, read_buff, res);

        f_close(&fp);
    }
#endif

#if 1
    {
        FIL fp;
        res = f_open(&fp, "b", FA_CREATE_NEW | FA_WRITE | FA_READ);
        if (!fp.obj.fs)
            printf("fp obj fs is NULL\n");
        printf("f_open res:%d\n", res);

        int bw;
        res = f_write(&fp, "123123123123", strlen("123123123123"), &bw);
        printf("write data(%d)  f_write res:%d\n", bw, res);

        res = f_sync(&fp);
        printf("f_sync res:%d\n", res);

        uint8_t read_buff[100];
        int br;
        f_lseek(&fp, 0);

        res = f_read(&fp, read_buff, sizeof(read_buff), &br);
        printf("read 1:a data(%d):%s  f_read res:%d\n", br, read_buff, res);

        f_close(&fp);
    }
#endif

    // printf("\n\n\n");
    // f_unmount("");

    return 0;
}