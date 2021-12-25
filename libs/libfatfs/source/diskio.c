/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#include <stdio.h>
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;

    printf("Get disk status:%d\n", stat);

	return stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define DISK_FILE_SIZE  (1024 * 1024 * 5)      // Flash总容量
const char *disk_file = "fatfs_vdisk";
#define PRINF_HEX_ARR(str,buf,len)\
do{\
    char *buff = (char *)buf;\
    printf("\e[32m[%s](%d):\e[0m", str, len);\
    for (int i = 0;i < len; ++i)\
    {\
        printf("0x%.2X ", buff[i] & 0xff);\
    }\
    printf("\r\n");\
} while (0);
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

    if (access(disk_file, F_OK))
    {
        printf("Create file %s\n", disk_file);
        creat(disk_file, 0666);
    }
    stat = RES_OK;

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;

    printf("pdrv:%d sector:%d  count:%d", pdrv, sector, count);
    if (!access(disk_file, F_OK))
    {
        int fd = open(disk_file, O_RDWR);
        if (fd)
        {
            lseek(fd, FF_MIN_SS * sector, SEEK_SET);
            int len = read(fd, buff, FF_MIN_SS * count);
            printf("\e[32m[%s](%d):\e[0m", "read", len);
            for (int i = 0;i < len; ++i)
            {
                printf("0x%.2X ", buff[i] & 0xff);
            }
            printf("\r\n");
            close(fd);
        }
    }
    res = RES_OK;

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;

    printf("pdrv:%d sector:%d  count:%d", pdrv, sector, count);
    if (!access(disk_file, F_OK))
    {
        int fd = open(disk_file, O_RDWR);
        if (fd)
        {
            lseek(fd, FF_MIN_SS * sector, SEEK_SET);
            int len = write(fd, buff, count * FF_MIN_SS);

            printf("write:%s\n", buff);
            printf("\e[32m[%s](%d):\e[0m", "write", len);
            for (int i = 0;i < len; ++i)
            {
                printf("0x%.2X ", buff[i] & 0xff);
            }
            printf("\r\n");

            close(fd);
        }
    }
    res = RES_OK;

	return res;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;

    printf("disk ioctl,pdrv:%d cmd:%d\n", pdrv, cmd);

    int fd = -1;
    if (!access(disk_file, F_OK))
        fd = open(disk_file, O_RDWR);
    if (fd < 0) return RES_NOTRDY;

    switch (cmd)
    {
    case CTRL_SYNC:
        fsync(fd);
        break;
    case GET_SECTOR_COUNT:
        *(DWORD *)buff = DISK_FILE_SIZE / 512;
        break;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = FF_MIN_SS;
        break;
    case GET_BLOCK_SIZE:
        *(WORD *)buff = 8;
        break;
    default:
        res = RES_PARERR; 
        break;
    }
    
    if (fd)
        close(fd);

	return res;
}

