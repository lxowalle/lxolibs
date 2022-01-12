#include "cell.h"

/**
 * @brief 创造一个细胞
 * @return
*/
static cell_t *_cell_create(cell_type_t type, cell_work_t work)
{
    int channel_num = 0;

    switch (type)
    {
    case CELL_TYPE_NORMAL:
        channel_num = 2;
        break;
    default:return NULL;
    }

    if (channel_num > CELL_CHANNEL_MAX)
        return NULL;

    cell_t *new_cell = (cell_t *)calloc(sizeof(cell_t), 1);
    if (!new_cell)
    {
        LOGE("Memory overflow!\n");
        return NULL;
    }

    new_cell->chnl_len = channel_num;
    new_cell->chnl = (cell_t **)calloc(sizeof(cell_t *), new_cell->chnl_len);
    if (!new_cell->chnl)
    {
        free(new_cell);
        LOGE("Memory overflow!\n");
        return NULL;
    }

    new_cell->type = type;
    new_cell->private = NULL;
    new_cell->work = work;

    return new_cell;
}

/**
 * @brief 消灭一个细胞
 * @return
*/
static void _cell_destory(cell_t **cell)
{
    if (*cell)
    {
        if ((*cell)->chnl)
            free((*cell)->chnl);
            
        free(*cell);
        *cell = NULL;
    }
}

/**
 * @brief 连通两个细胞
 * @return
*/
static err_t _cell_connect(cell_t *dst, cell_t *src, int bilateral)
{
    err_t err = OK;

    if (dst == NULL || src == NULL)
        return ERR_PARAM;

    if (src->chnl_len > CELL_CHANNEL_MAX)
        return ERR_TODO;

    /* bilateral=1,双向连通;bilateral=0,单向连通; */
    int index = 0;
    if (bilateral) {
        do
        {
            if (dst->chnl[index] == NULL)
            {
                dst->chnl[index] = src;
                break;
            }
            ++ index;
        }while(index < dst->chnl_len);
    }
 
    index = 0;
    do
    {
        if (src->chnl[index] == NULL)
        {
            src->chnl[index] = dst;
            break;
        }
        ++ index;
    }while(index < src->chnl_len);

    return err;
}

/**
 * @brief 断开两个细胞的连接
 * @return
*/
static err_t _cell_disconnect(cell_t *dst, cell_t *src, int bilateral)
{
    if (dst == NULL || src == NULL)
        return ERR_PARAM;

    /* bilateral=1,双向断开;bilateral=0,单向断开; */
    int index = 0;
    if (bilateral) {
        do
        {
            if (dst->chnl[index] == src)
            {
                dst->chnl[index] = NULL;
                break;
            }
            ++ index;
        }while(index < dst->chnl_len);
    }

    do
    {
        if (src->chnl[index] == dst)
        {
            src->chnl[index] = NULL;
            break;
        }
        ++ index;
    }while(index < src->chnl_len);

    return OK;
}

/**
 * @brief 打印细胞通道
 * @return
*/
static void _cell_list_channel(const cell_t *cell)
{
    int i = 0;
    printf("cell channel:\t");
    for (int i = 0; i < cell->chnl_len; i ++)
    {
        if (i % 3 == 0)
            printf("\n");
        printf("%p\t", cell->chnl[i]);
    }
    printf("\n");
}

/**
 * @brief 细胞工厂
*/
cell_factory_t cell_tools = 
{
    .create = _cell_create,
    .destory = _cell_destory,
    .connect = _cell_connect,
    .disconnect = _cell_disconnect,
    .list_channel = _cell_list_channel
};