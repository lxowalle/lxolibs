#include "mf_uartp.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

void uart_recv_handler(void)
{
    uint8_t data[256];
    mf_uartp_t *uartp = mf_uartp_choose(UARTP_TYPE_BIN);
    if (uartp)
    {
        fd_set rset;

        FD_ZERO(&rset);
        FD_SET(3, &rset);
        int rv = select(3 + 1, &rset, NULL, NULL, NULL);
        if (rv < 0)
        {
            // perror("Select error");
            return;
        }
        else if (rv == 0)
        {
            // perror("Select timeout");
            return;
        }

        int len = 0;
        mf_err_t err = uartp->ops.recv(uartp, data, sizeof(data), &len);
        if (err == MF_OK && len > 0)
        {
            printf("data:%s\n", data);
            uartp->ops.loop(uartp);
        }

#if 0
        len = linux_uart_read(3, sizeof(data), data);
        if (len)
        {
            LOGI("data(%d):%s\n", len, data);
        }
        if (len < 0)
        {
            perror("len < 0");
        }
#endif   
    }
}

int main(int argc, char *argv[])
{
    mf_err_t err = MF_ERR;
    mf_uartp_t *uartp = mf_uartp_choose(UARTP_TYPE_BIN);
    if (uartp)
    {
        err = uartp->ops.init(uartp);
        if (err != MF_OK)   {LOGE("uartp init is failed!\n");goto _exit;}
    }
    LOGI("Init ok!\n");


    while (1)
    {
        uart_recv_handler();

        static int cnt = 0;
        LOGI("+++%d\n", cnt ++);
        usleep(1000 * 1000);
    }

    uartp->ops.deinit(uartp);
    return 0;
_exit:
    LOGE("Err code:%d\n", err);
    return -1;
}