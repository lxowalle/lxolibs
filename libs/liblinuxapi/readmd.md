# Readme.md


记录测试代码

#### 目录操作

```c
#include <dirent.h>

void dir(void)
{

    DIR *dp;
    struct dirent *dirp;
    char *dir = "test_dir";

    system("mkdir test_dir");
    system("mkdir test_dir/a");
    system("mkdir test_dir/b");
    system("mkdir test_dir/c");

    /* 读目录 */
    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Can't open %s", dir);
    }
    else
    {
        printf("索引节点号\t");
        printf("偏移量\t");
        printf("文件名长度\t");
        printf("文件名\t");
        printf("文件类型\n");
        while ((dirp = readdir(dp)) != NULL)
        {
            printf("%ld\t", dirp->d_ino);
            printf("%ld\t", dirp->d_off);
            printf("%d\t", dirp->d_reclen);
            printf("%s\t", dirp->d_name);
            printf("%d\n", dirp->d_type);
        }
    }

    closedir(dp);
    system("rm -r test_dir");
}
```

#### 修改运行环境路径

```c
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
void test(void)
{
    /* 查看工作路径 */
    char *curr_work_dir1 = getcwd(NULL, 0);
    printf("curr wrok dir:%s\n", curr_work_dir1);
    /* 修改工作路径 */
    if (chdir("/"))
        fprintf(stderr, "Chdir error:%s\n", strerror(errno));

    char *curr_work_dir2 = getcwd(NULL, 0);
    printf("curr wrok dir:%s\n", curr_work_dir2);
    if (chdir(curr_work_dir1))
        fprintf(stderr, "Chdir error:%s\n", strerror(errno));
    free(curr_work_dir1);
    free(curr_work_dir2);
}
```

#### 终端输入/输出

```c
#include <unistd.h>
#include <errno.h>
#include <string.h>
void test(void)
{
    int c;
    while ((c = getc(stdin)) != EOF)
    {
        if (putc(c, stdout) == EOF)
            fprintf(stderr, "Puts error:%s\n", strerror(errno));
    }

    if (ferror(stdin))
        fprintf(stderr, "Gets error:%s\n", strerror(errno));

}

void test(void)
{
    char buf[200];

    printf("%% ");
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

        fputs(buf, stdout);
    }
}
```

#### 进程控制

```c
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
void test(void)
{
    printf("Get pid:%d\n", getpid());   
}

void test(void)
{
    char buf[200];
    pid_t pid;
    int status;

    printf("%% ");
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

        fputs(buf, stdout);

        if ((pid = fork()) < 0)
        {
            fprintf(stderr, "Fork error:%s\n", strerror(errno));
        }
        else if (pid == 0)
        {
            execlp(buf, buf, (char *) 0);
            fprintf(stderr, "Can't exec :%s\n", buf);
            exit(127);
        }

        if ((pid = waitpid(pid, &status, 0)) < 0)
            fprintf(stderr, "Waitpid error:%s\n", strerror(errno));
        printf("%% ");
    }
}

```

#### 信号量使用

```c
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
```