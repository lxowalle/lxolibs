#ifndef __DB_H
#define __DB_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/**
 *  Global View
 * 
 *  Sector size         = DB_SECTOR_SIZE
 *  Head size           = sizeof(db_head_t)
 *  None size           = DB_SECTOR_SIZE - sizeof(db_head_t)
 *  All face info size  = DB_FACE_MAX_NUM * 256(Valid size is sizeof(db_item_t))
 *  ___________________________________________________________________________
 * |      Sector0      |      Sector1      |      ...      |      SectorN      |
 * |___________________|___________________|_______________|___________________|
 * | Head |    None    |                      All face info                    |
 * |______|____________|_______________________________________________________|
*/
/* face info config */
#define DB_UID_SIZE         (16)
#define DB_FTR_SIZE         (196)
#define DB_ITEM_MAX_SIZE    (256)
#define DB_FACE_MAX_NUM     (2048)

/* Head config */
#define DB_HEAD_SIGN        (0xAA5555AA)
#define DB_HEAD_VERSION     (0x00000001)

/* Flash config */
#define DB_SECTOR_SIZE      (4096)
#define DB_START_ADDRESS    (0x0)
#define DB_ADDERSS_SIZE     (DB_FACE_MAX_NUM * 256 + DB_SECTOR_SIZE)
#define DB_ADDRESS_MAX_SIZE (0x81000)

typedef enum
{
    DB_STA_OK = 0,
    DB_STA_ERR,
    DB_STA_FULL,
    DB_STA_MAX
}db_status_t;

typedef enum
{
    DB_TYPE_FACE = 0,
    DB_TYPE_MAX
}db_type_t;

typedef enum
{
    MF_OK = 0,
    MF_ERR_PARAM,
    MF_ERR_MEM,
    MF_ERR_NORMAL,
    MF_ERR_REINIT,
    MF_ERR_UNINIT,
    MF_ERR_UNKNOWN,
    MF_ERR_TODO,
    MF_ERR_INIT,
    MF_ERR_MAX
}db_err_t;

typedef struct
{
    uint32_t version;
    uint32_t total;
    uint32_t checksum;
    uint32_t index[DB_FACE_MAX_NUM / 32 + 1];
    uint32_t sign;
}db_header_t;

typedef struct
{
    uint32_t index;
    uint8_t uid[DB_UID_SIZE];
    uint8_t ftr[DB_FTR_SIZE];
}db_item_t;

typedef struct
{
    uint8_t is_init : 1;
    db_status_t status;
    void *private;

    void (*lock)(void);
    void (*unlock)(void);
    db_err_t (*init)(void);
    db_err_t (*deinit)(void);
    db_err_t (*insert)(uint8_t *uid, void *item);
    db_err_t (*delete)(uint8_t *uid);
    db_err_t (*select)(uint8_t *uid, void* item);
    db_err_t (*update)(uint8_t *uid, void* item);
    void (*iterate_init)(void);
    int (*iterate)(void);
    int (*num)(void);

    db_err_t (*control)(int cmd, ...);
}db_t;

extern int db_write(uint32_t addr, uint8_t *data, int len);
extern int db_read(uint32_t addr, uint8_t *data, int len);
extern db_t db;

db_err_t db_choose(db_type_t type);

#endif /* __DB_H */