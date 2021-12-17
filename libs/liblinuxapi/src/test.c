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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ulimit.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <ftw.h>

void test(void)
{

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