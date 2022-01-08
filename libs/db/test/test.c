#include "db.h"
#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int disk_initialize (void);
int db_read(uint32_t address, uint8_t *buff, int cnt);
int db_write(uint32_t address, uint8_t *buff, int cnt);
int disk_deinitialize (void);

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

uint8_t* get_rand_ftr(void)
{
#define FTR_LEN     DB_FTR_SIZE
    static uint8_t ftr[FTR_LEN] = {0};
    for(int i = 0;i < FTR_LEN;i ++)
    {
        ftr[i]=(uint8_t)rand();
    }
    // LOGHEX("rand ftr", ftr, FTR_LEN);
    return ftr;
}

static void __attribute__((constructor)) facedb_test(void)
{
#if 1
    // Use file to test facedb
    disk_initialize();
#endif

#if 1
    LOGI("Start DB test!\n");

    db_err_t err = MF_OK;
    err = db_choose(DB_TYPE_FACE);
    if (MF_OK != err) {LOGE("Error %d\n", err); exit(-1);}
    err = db.init();
    if (MF_OK != err) {LOGE("Error %d\n", err); exit(-1);}

    LOGI("Insert&Select&Update&Delete face test!\n");
    for (int i = 0; i < DB_FACE_MAX_NUM; i ++)
    {
        uint8_t *rand_ftr = get_rand_ftr();
        uint8_t *uid = db_id2uid(i);
        err = db.insert(uid, rand_ftr);
        if (MF_OK != err) {LOGW("Can't insert %d, err=%d\n", i, err); exit(-1);}

        uint8_t select_ftr[DB_FTR_SIZE];
        err = db.select(uid, &select_ftr);
        if (MF_OK != err) {LOGW("Can't select %d, err=%d\n", i, err); exit(-1);}
        if (memcmp(select_ftr, rand_ftr, DB_FTR_SIZE))
        {
            LOGE("Select %d error, bad value!\n", i);
            LOGHEX("Insert ftr", rand_ftr, DB_FTR_SIZE);
            LOGHEX("Select ftr", select_ftr, DB_FTR_SIZE);
            exit(-1);
        }

        rand_ftr = get_rand_ftr();
        err = db.update(uid, rand_ftr);
        if (MF_OK != err) {LOGW("Can't update %d, err=%d\n", i, err); exit(-1);}

        err = db.select(uid, &select_ftr);
        if (MF_OK != err) {LOGW("Can't select %d, err=%d\n", i, err); exit(-1);}
        if (memcmp(select_ftr, rand_ftr, DB_FTR_SIZE))
        {
            LOGE("Select %d error, bad value!\n", i);
            LOGHEX("Insert ftr", rand_ftr, DB_FTR_SIZE);
            LOGHEX("Select ftr", select_ftr, DB_FTR_SIZE);
            exit(-1);
        }

        err = db.delete(uid);
        if (MF_OK != err) {LOGW("Can't Delete %d, err=%d\n", i, err); exit(-1);}
        err = db.select(uid, &select_ftr);
        if (MF_ERR_DB != err) {LOGW("Select %d? meaning is havn't delete it, err=%d\n", i, err); exit(-1);}
    }
    LOGI("Total face num: %d\n", db.num());

    err = db.deinit();
    if (MF_OK != err) {LOGE("Error %d\n", err); exit(-1);}

    LOGI("DB test is OK!\n");
#endif

#if 0
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
    LOGI("There is db test!\n");

    return 0;
}

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
            // LOGI("Read address:%#x, cnt:%d\n", address, cnt);
            // LOGHEX("Read data", buff, cnt);
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
            // LOGI("Write address:%#x, cnt:%d\n", address, cnt);
            // LOGHEX("Write data", buff, cnt);
            close(fd);
            return len;
        }
    }
    
	return 0;
}

int disk_deinitialize (void)
{

	return 0;
}
