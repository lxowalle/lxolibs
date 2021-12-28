#include "mf_uartp.h"
#include "mf_uartp_bin.h"

/**
 * @brief 选择串口协议
 * @return 
*/
mf_uartp_t *mf_uartp_choose(uartp_type_t type)
{
    mf_uartp_t *uartp = NULL;

    switch (type)
    {
    case UARTP_TYPE_BIN:
        uartp = get_uartp_bin();
        break;
    default:
        uartp = NULL;
        break;
    }

    return uartp;
}
