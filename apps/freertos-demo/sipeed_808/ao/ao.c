#include "ao.h"
#include "ao_normal.h"

ao_t ao;

ao_err_t ao_choose(ao_type_t type)
{
    ao_err_t err = AO_OK;

    switch (type)
    {
        case AO_TYPE_NORMAL:
            memcpy(&ao, get_ao_normal_handle(), sizeof(ao_t));
            break;
        default:
            err = AO_ERR_PARAM;
            break;
    }

    return err;
}

