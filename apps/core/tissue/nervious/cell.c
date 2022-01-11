#include "cell.h"

/**
 * @brief 创造一个细胞
 * @return
*/
static cell_t *_cell_create(cell_type_t type, cell_work_t doing)
{
    cell_t *new_cell = (cell_t *)calloc(sizeof(cell_t), 1);
    if (!new_cell)
    {
        LOGE("Memory overflow!\n");
        return NULL;
    }

    switch (type)
    {
    case CELL_TYPE_NORMAL:
        break;
    default:return NULL;
    }

    new_cell->type = type;
    new_cell->private = NULL;
    new_cell->work = doing;

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

    if (src->chnl_len >= CELL_CHANNEL_MAX)
        return ERR_TODO;

    /* dir=0,单向连通;dir=1,双向连通 */
    if (bilateral) {
        if (dst->chnl_len >= CELL_CHANNEL_MAX)
            return ERR_TODO;

        dst->chnl[dst->chnl_len ++] = src;
        src->chnl[src->chnl_len ++] = dst;
    }
    else {
        src->chnl[src->chnl_len ++] = dst;
    }
    
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

    if (bilateral)
    {
        for (int i = 0; i < dst->chnl_len; i ++)
        {
            if (dst->chnl[i] == src)
                dst->chnl[i] = NULL;
        }
    }

    for (int i = 0; i < src->chnl_len; i ++)
    {
        if (src->chnl[i] == dst)
            src->chnl[i] = NULL;
    }

    return OK;
}

/**
 * @brief 细胞工厂
*/
cell_factory_t cell_facetory = 
{
    .create = _cell_create,
    .destory = _cell_destory,
    .connect = _cell_connect,
    .disconnect = _cell_disconnect
};