#include "aiao.h"
#include "aiao_normal.h"

aiao_t aiao;

aiao_err_t aiao_choose(aiao_type_t type)
{
    aiao_err_t err = AIAO_OK;

    switch (type)
    {
        case AIAO_TYPE_NORMAL:
            memcpy(&aiao, get_aiao_normal_handle(), sizeof(aiao_t));
            break;
        default:
            err = AIAO_ERR_PARAM;
            break;
    }

    return err;
}

