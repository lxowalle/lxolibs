#include "../inc/mf_uartp.h"
#include "../inc/mf_uartp_bin.h"

mf_uartp_t mf_uartp;

/**
 * @brief 选择串口协议
 * @return 
*/
mf_err_t mf_uartp_choose(uartp_type_t type)
{
    mf_err_t err = MF_OK;

    switch (type)
    {
    case UARTP_TYPE_BIN:
        memcpy(&mf_uartp, get_uartp_bin(), sizeof(mf_uartp));
        err = MF_OK;
        break;
    default:
        err = MF_ERR_UNINIT;
        break;
    }

    return err;
}
