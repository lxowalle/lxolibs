#include "inc/facedb.h"

static db_t facedb;

#if (DB_ADDRESS_MAX_SIZE < DB_ADDERSS_SIZE)
    #error "Flash size is too small for facedb!"
#endif

typedef struct
{
    uint8_t need_init : 1;
    db_header_t header;
}facedb_private_t;

static db_err_t _facedb_init(void)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;

    if (db->is_init)    return MF_ERR_REINIT;
    /* Init lock */
    // ...

    /* Lock */
    if (db->lock)
        db->lock();

    /* Add private param */
    static facedb_private_t private;
    db->private = &private;
    
    /* Init database */
    uint8_t head_sector[DB_SECTOR_SIZE] = {0};
    int len = db_read(DB_START_ADDRESS, (uint8_t *)&private.header, sizeof(private.header));
    if (len != sizeof(private.header))
    {
        if (db->unlock)
            db->unlock();
        return MF_ERR_UNKNOWN;
    }

    // Check sign
    if (private.header.sign != DB_HEAD_SIGN)
    {
        if (db->unlock)
            db->unlock();
        goto _need_init_head;
    }

    // Check version
    if (private.header.version != DB_HEAD_VERSION)
    {
        if (db->unlock)
            db->unlock();
        goto _need_init_head;
    }

    // Check checksum
    uint32_t checksum = private.header.version
                    + private.header.total
                    + private.header.sign;
    for (int i = 0; i < sizeof(private.header.index) / sizeof(private.header.index[0]); i ++)
    {
        checksum += private.header.index[i];
    }
    if (private.header.checksum != checksum)
    {
        if (db->unlock)
            db->unlock();
        goto _need_init_head;
    }

    // If check is ok, header is need not init, jump next step
    goto _need_not_init_head;

_need_init_head:
    private.header.total = 0;
    private.header.sign = DB_HEAD_SIGN;
    private.header.version = DB_HEAD_VERSION;
    memset(private.header.index, 0, sizeof(private.header.index) / sizeof(private.header.index[0]));
    private.header.checksum = private.header.sign
                            + private.header.version;   // Need not add total and index, because they are 0
    len = db_write(DB_START_ADDRESS, (uint8_t *)&private.header, sizeof(private.header));
    if (len != sizeof(private.header))
    {
        if (db->unlock)
            db->unlock();
        return MF_ERR_UNKNOWN;
    }
_need_not_init_head:

    /* Init over */
    db->status = DB_STA_OK;
    db->is_init = 1;

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_deinit(void)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Deinit over */
    db->is_init = 0;

    /* Unlock */
    if (db->unlock)
        db->unlock();

    /* Deinit lock */
    // ...

    return err;
}

static db_err_t _facedb_insert(uint8_t *uid, void *item)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    // ...

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_delete(uint8_t *uid)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    // ...

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_select(uint8_t *uid, void* item)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    // ...

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_update(uint8_t *uid, void* item)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    // ...

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_iterate_init(void)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    // ...

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_iterate()
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    // ...

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_num()
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    // ...

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_t facedb = 
{
    .is_init = 0, 
    .status = DB_STA_ERR,
    .private = NULL,

    .init = _facedb_init,
    .deinit = _facedb_deinit,
    .insert = NULL,
    .delete = NULL,
    .select = NULL,
    .update = NULL,
    .iterate_init = NULL,
    .iterate = NULL,
    .num = NULL
};

db_t *get_facedb_handle(void)
{
    return &facedb;
}