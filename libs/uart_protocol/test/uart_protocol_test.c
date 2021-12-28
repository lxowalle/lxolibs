#include "mf_uartp.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    mf_err_t err = MF_ERR;
    mf_uartp_t *uartp = mf_uartp_choose(UARTP_TYPE_BIN);
    if (uartp)
    {
        err = uartp->ops.init(uartp);
        if (err != MF_OK)   {LOGE("uartp init is failed!\n");goto _exit;}
    }
   

    return 0;
_exit:
    LOGE("Err code:%d\n", err);
    return -1;
}