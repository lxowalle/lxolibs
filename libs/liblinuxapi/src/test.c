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

#include <sys/stat.h>

void test(void)
{
    char *dir1 = "file1";

    int fd;
    fd = open(dir1, O_RDWR | O_CREAT| O_TRUNC, 0777);
    if (fd)
    {
        struct stat s;
        lstat(dir1, &s);

        struct timespec times[2];
        times[0] = s.st_atim;
        times[1] = s.st_mtim;

        /**
         * 使用futimens修改文件的访问时间和修改时间
         * 
         * struct timespec time;
         * time.tv_nsec设为UTIME_NOW时，futimens会忽略tv_sec字段，并将相应时间戳设为当前时间
         * time.tv_nsec设为UTIME_OMIT时，futimens会忽略tv_sec字段，并且不修改时间戳
         * 
         * ls -lu xxx   查看最后访问时间
         * ls -lc xxx   查看最后修改时间
        */
        futimens(fd, times);

        if (close(fd) < 0)
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }

    if (!access(dir1, F_OK))
    {
        if (remove(dir1) < 0)
            fprintf(stderr, "Remove error:%s\n", strerror(errno));
    }
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