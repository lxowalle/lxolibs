#include "inc/facedb.h"

static db_t facedb;

#if (DB_ADDRESS_MAX_SIZE < DB_ADDERSS_SIZE)
    #error "Flash size is too small for facedb!"
#endif

#if (DB_SECTOR_SIZE % DB_ITEM_MAX_SIZE != 0)
    #error "The face item is not aligned with the sectors!"
#endif

typedef struct
{
    uint8_t need_init : 1;
    db_header_t header;
}facedb_private_t;

#define _header_index_set(index)

static uint32_t _get_header_index_bit(db_header_t *header, uint32_t index)
{
    return ((header->index[index / 32] >> (index % 32)) & 0x01);
}

static uint32_t _get_face_num_in_header(db_header_t *header)
{
    uint32_t face_num = 0;
    for (int i = 0; i < sizeof(header->index) / sizeof(header->index[0]); i ++)
    {
        uint32_t tmp = header->index[i];
        while (tmp)
        {
            if (tmp & 0x1)
                face_num +=1;
            tmp >>= 1;
        }
    }

    return face_num;
}

static uint32_t _get_header_checknum(db_header_t *header)
{
    uint32_t checksum = header->version
                    + header->total
                    + header->sign;
    for (int i = 0; i < sizeof(header->index) / sizeof(header->index[0]); i ++)
    {
        checksum += header->index[i];
    }

    // printf("%#x  %#x\n", header->checksum, checksum);

    return checksum;
}

/**
 * @brief Update and save header
 * @details 
 * @param   [in]    header
 * @param   [in]    type    If type = 1, Set the bit of the index and update header info;
 *                          If type = -1, Clear the bit of the index and update header info;
 *                          If type = other, Just update header info                      
 * @param   [in]    index   Face index of header.index[]
 * 
 * @return 
*/
static db_err_t _update_header(db_header_t *header, int type, uint32_t index)
{
    db_err_t err = MF_OK;

    if (type == 1)
    {
        if (header->total >= DB_FACE_MAX_NUM)
            return MF_ERR_DB;
        header->total += 1;
        header->index[index / 32] |= (1 << (index % 32));
    }
    else if (type == -1)
    {
        if (header->total == 0)
            return MF_ERR_DB;
        header->total -= 1;
        header->index[index / 32] &= ~(1 << (index % 32));
    }


    header->checksum = _get_header_checknum(header);

    int len = db_write(DB_START_ADDRESS, (uint8_t *)header, sizeof(db_header_t));
    if (len != sizeof(db_header_t))
    {
        err = MF_ERR_DB;
        goto _exit;
    }

_exit:
    return err;
}

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
    // Check item size
    if (sizeof(db_item_t) > DB_ITEM_MAX_SIZE)
    {
        if (db->unlock)
            db->unlock();
        return MF_ERR_TODO;
    }

    // Read header info
    uint8_t head_sector[DB_SECTOR_SIZE] = {0};
    int len = db_read(DB_START_ADDRESS, (uint8_t *)&private.header, sizeof(private.header));
    if (len != sizeof(private.header))
    {
        if (db->unlock)
            db->unlock();
        return MF_ERR_UNKNOWN;
    }

    // Check sign for header
    if (private.header.sign != DB_HEAD_SIGN)
    {
        if (db->unlock)
            db->unlock();
        goto _need_init_head;
    }

    // Check version for header
    if (private.header.version != DB_HEAD_VERSION)
    {
        if (db->unlock)
            db->unlock();
        goto _need_init_head;
    }

    // Check facenum for header
    uint32_t face_num = _get_face_num_in_header(&private.header);
    if (private.header.total != face_num)
    {
        if (db->unlock)
            db->unlock();
        goto _need_init_head;
    }

    // Check checksum for header
    uint32_t checksum = _get_header_checknum(&private.header);
    if (private.header.checksum != checksum)
    {
        if (db->unlock)
            db->unlock();
        goto _need_init_head;
    }
    // If check is ok, header is need not init, jump next step
    goto _need_not_init_head;

_need_init_head:
    memset(&private.header, 0, sizeof(private.header));
    private.header.sign = DB_HEAD_SIGN;
    private.header.version = DB_HEAD_VERSION;
    err = _update_header(&private.header, 0, 0);
    if (MF_OK != err)
    {
        if (db->unlock)
            db->unlock();
        return err;
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
    facedb_private_t *private = (facedb_private_t *)db->private;

    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Insert */
    uint8_t *ftr = (uint8_t *)item;
    db_item_t db_item;
    uint32_t idx;
    int len;
    memcpy(db_item.uid, uid, DB_UID_SIZE);
    memcpy(db_item.ftr, ftr, DB_FTR_SIZE);
    idx = db_uid2id(db_item.uid);

    // Check if idx overflow
    if (idx >= DB_FACE_MAX_NUM)
    {
        err = MF_ERR_PARAM;
        goto _exit;
    }

    // Check if idx has been set
    if (_get_header_index_bit(&private->header, idx) != 0)    
    {
        err = MF_ERR_DB;
        goto _exit;
    }

    // Save
    uint32_t address = idx * DB_ITEM_MAX_SIZE + DB_HEADER_MAX_SIZE;
    len = db_write(address, (uint8_t *)&db_item, sizeof(db_item_t));
    if (len != sizeof(db_item_t))
    {
        err = MF_ERR_DB;
        goto _exit;
    }

    // Update header
    err = _update_header(&private->header, 1, idx);
    if (MF_OK != err)
    {
        if (db->unlock)
            db->unlock();
        return err;
    }

_exit:
    /* Unlock */
    if (db->unlock)
        db->unlock();

    return err;
}

static db_err_t _facedb_delete(uint8_t *uid)
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    facedb_private_t *private = (facedb_private_t *)db->private;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Delete */
    uint32_t idx = db_uid2id(uid);

    // Check if idx overflow
    if (idx >= DB_FACE_MAX_NUM)
    {
        err = MF_ERR_PARAM;
        goto _exit;
    }

    // Check if idx is not set
    if (_get_header_index_bit(&private->header, idx) == 0)    
    {
        err = MF_ERR_DB;
        goto _exit;
    }

    // Delete from header
    err = _update_header(&private->header, -1, idx);
    if (MF_OK != err)
    {
        if (db->unlock)
            db->unlock();
        return err;
    }

_exit:
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

static int _facedb_iterate()
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

static int _facedb_num()
{
    db_err_t err = MF_OK;
    db_t *db = (db_t *)&facedb;
    facedb_private_t *private = (facedb_private_t *)db->private;
    if (!db->is_init)    return MF_ERR_UNINIT;

    /* Lock */
    if (db->lock)
        db->lock();

    /* Handler */
    int num = private->header.total;

    /* Unlock */
    if (db->unlock)
        db->unlock();

    return num;
}

static db_t facedb = 
{
    .is_init = 0, 
    .status = DB_STA_ERR,
    .private = NULL,

    .init = _facedb_init,
    .deinit = _facedb_deinit,
    .insert = _facedb_insert,
    .delete = _facedb_delete,
    .select = _facedb_select,
    .update = _facedb_update,
    .iterate_init = _facedb_iterate_init,
    .iterate = _facedb_iterate,
    .num = _facedb_num
};

db_t *get_facedb_handle(void)
{
    return &facedb;
}

int __attribute__((weak)) db_write(uint32_t addr, uint8_t *data, int len)
{
    printf("Warnning, int(*db_write)(uint32_t addr,uint8_t *data,int len) not define for facedb!\n");
    return len;
}

int __attribute__((weak)) db_read(uint32_t addr, uint8_t *data, int len)
{
    printf("Warnning, int(*db_read)(uint32_t addr,uint8_t *data,int len) not define for facedb!\n");
    return len;
}