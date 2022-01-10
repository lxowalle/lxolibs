#include "vi.h"
#include "vi_dual_cam.h"

vi_t vi;

/**
 * @brief 选择串口协议
 * @return 
*/
vi_err_t vi_choose(vi_type_t type)
{
    vi_err_t err = VI_OK;

    switch (type)
    {
    case VI_TYPE_USB_CAM:
        memcpy(&vi, get_vi_dual_cam_handle(), sizeof(vi_t));
        err = VI_OK;
        break;
    default:
        err = VI_ERR_UNKNOWN;
        break;
    }

    return err;
}





