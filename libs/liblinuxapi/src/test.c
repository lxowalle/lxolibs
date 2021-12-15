#include "liblinuxapi.h"
#include <stdio.h>

// #include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
void sig_int_cb(int signo)
{
    printf("sig int callback\n");
    exit(0);
}

void test(void)
{
    if (signal(SIGINT, sig_int_cb) == SIG_ERR)
        fprintf(stderr, "Signal error:%s\n", strerror(errno));
}

int main(void)
{

    test();


    int c = 0;
    while ((c = getc(stdin)) != EOF)
    {
        if (putc(c, stdout) == EOF)
            fprintf(stderr, "Puts error:%s\n", strerror(errno));
    }
    return 0;
}