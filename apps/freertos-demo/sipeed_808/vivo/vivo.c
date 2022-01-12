#include "vivo.h"
#include "vivo_dual_cam.h"

vivo_t vivo;

/**
 * @brief 选择串口协议
 * @return 
*/
vivo_err_t vivo_choose(vivo_type_t type)
{
    vivo_err_t err = VIVO_OK;

    switch (type)
    {
    case VIVO_TYPE_USB_CAM:
        memcpy(&vivo, get_vivo_dual_cam_handle(), sizeof(vivo_t));
        err = VIVO_OK;
        break;
    default:
        err = VIVO_ERR_UNKNOWN;
        break;
    }

    return err;
}





