#include "flow.h"
#include "flow_normal.h"

flow_t flow;

flow_err_t flow_choose(flow_type_t type)
{
    flow_err_t err = FLOW_OK;

    switch (type)
    {
    case FLOW_TYPE_NORMAL:
        memcpy(&flow, get_flow_normal_handle(), sizeof(flow_t));
        break;
    default:
        err = FLOW_ERR_PARAM;
        break;
    }

    return err;
}

