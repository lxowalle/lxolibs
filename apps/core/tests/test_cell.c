#include "cell.h"
#include "common.h"

void cell_work(struct cell_t *cell, void *param)
{
    LOGI("In cell!\n");
    for (int i = 0; i < cell->chnl_len; i ++)
    {
        if (cell->chnl[i] && cell->chnl[i]->work)
            cell->chnl[i]->work(cell->chnl[i], param);
    }
}

int main(int argc, char *argv[])
{
    LOGI("test cell..\n");

    cell_t *cell1 = cell_tools.create(CELL_TYPE_NORMAL, cell_work);
    if (!cell1)   LOGE("Error!\n");

    cell_t *cell2 = cell_tools.create(CELL_TYPE_NORMAL, cell_work);
    if (!cell2)   LOGE("Error!\n");
    LOGI("cell size:%ld\n", sizeof(cell_t));

    LOGI("---- connect ----\n");
    cell_tools.connect(cell1, cell2, 0);
    cell2->work(cell2, NULL);
    cell_tools.list_channel(cell2);

    LOGI("---- disconnect ----\n");
    cell_tools.disconnect(cell1, cell2, 0);
    cell2->work(cell2, NULL);
    cell_tools.list_channel(cell2);

    LOGI("---- destory ----\n");
    cell_tools.destory(&cell1);
    cell_tools.destory(&cell2);

    if (cell1)
        LOGW("Can't delete!\n");
    if (cell2)
        LOGW("Can't delete!\n");  
    return 0;
}
