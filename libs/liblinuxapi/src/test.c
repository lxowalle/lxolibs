#include "liblinuxapi.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/times.h>

static void pr_times(clock_t, struct tms *, struct tms *);
static void do_cmd(char *);

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    printf("argc:%d\n", argc);
    for (int i = 0; i < argc - 1; i ++)
    {
        do_cmd(argv[i]);
    }
    
    exit(0);
}

static void do_cmd(char *cmd)
{
    struct tms tmsstart, tmsend;
    clock_t start, end;
    int status;

    printf("\ncommand: %s\n", cmd);

    if ((start = times(&tmsstart)) == -1)
        fprintf(stderr, "times error:%s\n", strerror(errno));
    
    if ((status = system(cmd)) == -1)
        fprintf(stderr, "system error:%s\n", strerror(errno));

    if ((end = times(&tmsend)) == -1)
        fprintf(stderr, "times error:%s\n", strerror(errno));

    pr_times(end - start, &tmsstart, &tmsend);
}

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
    static long clktck = 0;

    if (clktck == 0)
        if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
            fprintf(stderr, "sysconf error:%s\n", strerror(errno));
        
        printf(" Real time                          : %7.2f\n", real / (double) clktck);
        printf(" User CPU time                      : %7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
        printf(" System CPU time                    : %7.2f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double)clktck);
        printf(" User CPU time of dead children     : %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime) / (double)clktck);
        printf(" System CPU time of dead children   : %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime) / (double)clktck);
}