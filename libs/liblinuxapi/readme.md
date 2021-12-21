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

#### 文件操作

Linux有很多函数会有一个带有at后缀的函数，例如`open`和`openat`，区别是open默认通过AT_FDCWD宏来设置以当前路径作为相对路径的起点，`openat`允许用户传入一个文件描述符来设置相对路径的起点。
文件操作中，write、read都是从offset递增的读写，偏移值通过lseek设置。

在打开文件时设置O_APPEND标志来实现多个进程写同一文件时的原子操作


```c

void test(void)
{
    char *dir = "file";

    if (creat(dir, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        char buf1[20] = "123455666789";
        char buf2[20] = "abcdefghijk";
        if (write(fd, buf1, sizeof(buf1)) != sizeof(buf1))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        off_t off = lseek(fd, -sizeof(buf1), SEEK_CUR);
        if (off == -1)  // Don't test it is less than 0
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (read(fd, buf2, sizeof(buf2)) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        printf("read str:%s\n", buf2);

        off = lseek(fd, 10, SEEK_CUR);
        if (off == -1)  // Don't test it is less than 0
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (write(fd, buf2, sizeof(buf2)) != sizeof(buf2))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        // 使用命令读文件实际内容`od xxx`，显示字符`od -c xxx`,显示十六进制`od -h xxx`

        close(fd);
        fd = -1;
    }
}
```

**多进程/线程时文件操作需要添加原子操作的情况：**
1. 多进程时向文件末尾数据，会出现使用lseek时会误判了当前文件末尾的位置，导致某个进程写的数据丢失。解决方法：每次写数据前调用lseek设置偏移量为文件末尾，或者在调用open时添加O_APPEND让每次写数据都在文件末尾写入。
2. 多进程时读写文件的某个位置时，出现读/写冲突。解决方法：使用fwrite、fread来在指定偏移位置读写数据
   
```c
void test(void)
{
    char *dir = "file";

    if (creat(dir, 0777) == -1)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        char buf1[20] = "123455666789";
        char buf2[20] = "abcdefghijk";
        if (pwrite(fd, buf1, sizeof(buf1), 25) != sizeof(buf1))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (pwrite(fd, buf2, sizeof(buf2), 10) != sizeof(buf2))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));

        if (pread(fd, buf1, sizeof(buf1), 25) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        printf("pread res:%s\n", buf1);

        if (pread(fd, buf1, sizeof(buf1), 10) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        printf("pread res:%s\n", buf1);

        close(fd);
        fd = -1;
    }
}
```

3. 多进程创建文件时，会出现文件已存在导致创建失败。解决方法：创建文件前使用open来检查该文件是否存在；或者直接在调用open时添加O_CREAT参数

**使用dup复制文件描述符，用来给多个进程共享同一个文件资源**

```c
#include <unistd.h>
void test(void)
{
    char *dir = "file";

    if (creat(dir, 0777) == -1)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        int fd_new = dup(fd);
        printf("fd:%d  fd_new:%d\n", fd, fd_new);

        int fd_new2 = dup2(fd, 20);         // 会关闭原来fd=20的文件
        printf("fd:%d  fd_new2:%d\n", fd, fd_new2);

        int fd_new3 = dup2(fd, fd);
        printf("fd:%d  fd_new2:%d\n", fd, fd_new3);

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new2))
            fprintf(stderr, "Close error:%s\n", strerror(errno));

        /* There will return error, because fd_new3=fd had closed */
        if (close(fd_new3))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**文件数据的同步**

```c
#include <unistd.h>
void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        sync();         // 将块缓冲写入队列，但不保证写入磁盘
        fsync(fd);      // 将文件的数据写入磁盘，并同步更新文件属性
        fdatasync(fd);  // 将文件写入的数据写入磁盘

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```
**修改已打开的文件属性**

通过fcntl函数来修改以及打开的文件属性，共有5种功能：
1. 复制已有的描述符(F_DUPFD/F_DUPFD_CLOEXEC)
2. 获取/设置文件描述符标志(F_GETFD/F_SETFD)
3. 获取/设置文件状态标志(F_GETFL/F_SETFL)
4. 获取/设置异步I/O所有权(F_GETOWN/F_SETOWN)
5. 获取/设置记录锁(F_GETLK/F_SETLK/F_SETLKW)

```c
#include <fcntl.h>
/* 文件状态值解析 */
void read_fd_state(int sta)
{
    switch (sta & O_ACCMODE)
    {
    case O_RDONLY:
    {
        printf("readonly");
        break;
    }
    case O_WRONLY:
    {
        printf("writeonly");
        break;
    }
    case O_RDWR:
    {
        printf("readwrite");
        break;
    }
    default:
    {
        printf("unknown");
        break;
    }
    }

    if (sta & O_APPEND)
        printf(",append");
    if (sta & O_NONBLOCK)
        printf(",nonblock");
    if (sta & O_SYNC)
        printf(",sync");
    if (sta & O_FSYNC)
        printf(",fsync");
    if (sta & O_CREAT)
        printf(",creat");

    printf("\n");
}

void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        int fd_flag = fcntl(fd, F_GETFD);           // 1. 获取文件描述符标志
        printf("fd:%d fd_flag:%d\n", fd, fd_flag);
        fcntl(fd, F_SETFD, 1);                      // 2. 设置文件描述符标志，这里包括了FD_CLOEXEC标志

        int fd_new = fcntl(fd, F_DUPFD);            // 3. 复制文件描述符，清除了FD_CLOEXEC标志，该标志表示文件在执行时该描述符依然有效
        printf("fd:%d  fd_new:%d\n", fd, fd_new);

        int fd_new2= fcntl(fd, F_DUPFD_CLOEXEC);    // 4. 复制文件描述符，不清除FD_CLOEXEC标志
        printf("fd:%d  fd_new2:%d\n", fd, fd_new2);

        // 这里是测试F_DUPFD和F_DUPFD_CLOEXEC是否清除FD_CLOSE标志
        fd_flag = fcntl(fd, F_GETFD);
        printf("fd:%d fd_flag:%d\n", fd, fd_flag);
        fd_flag = fcntl(fd_new, F_GETFD);
        printf("fd_new:%d fd_flag:%d\n", fd_new, fd_flag);
        fd_flag = fcntl(fd_new2, F_GETFD);
        printf("fd_new2:%d fd_flag:%d\n", fd_new2, fd_flag);

        int fd_sta = fcntl(fd, F_GETFL);            // 5. 获取文件状态
        read_fd_state(fd_sta);

        fd_sta |= O_NONBLOCK;
        fd_sta |= O_SYNC;
        fcntl(fd, F_SETFL, fd_sta);                 // 6. 设置文件状态（测试不能更改O_SYNC，O_FSYNC标志）
        fd_sta = fcntl(fd, F_GETFL);                // 获取文件状态
        read_fd_state(fd_sta);

        int fd_own = fcntl(fd, F_GETOWN);           // 7. 获取当前接收SIGIO和SIGURG信号的进程ID或进程组ID，返回值小于0时，其绝对值表示进程组ID
        printf("fd_own:%d\n", fd_own);
        fcntl(fd, F_SETOWN, 2000);                  // 8. 设置当前接收SIGIO和SIGURG信号的进程ID或进程组ID

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
        if (close(fd_new2))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```
**关于ioctl**

ioctl有非常多的命令来操作I/O,需要根据实际的情况来选择和使用命令

```c
#include <unistd.h>
#include <sys/ioctl.h>


// ...
```

#### 文件与目录

** 查看文件类型 **

使用stat、fstat、lstat来查看文件的类型

```c
#include <sys/stat.h>
void test(void)
{
    char *dir = "file";

    /**
    struct stat {
        mode_t     st_mode;       //文件对应的模式，文件，目录等
        ino_t      st_ino;        //inode节点号
        dev_t      st_dev;        //设备号码
        dev_t      st_rdev;       //特殊设备号码
        nlink_t    st_nlink;      //文件的连接数
        uid_t      st_uid;        //文件所有者
        gid_t      st_gid;        //文件所有者对应的组
        off_t      st_size;       //普通文件，对应的文件字节数
        time_t     st_atime;      //文件最后被访问的时间
        time_t     st_mtime;      //文件内容最后被修改的时间
        time_t     st_ctime;      //文件状态改变时间
        blksize_t st_blksize;     //文件内容对应的块大小
        blkcnt_t   st_blocks;     //文件内容对应的块数量
    };
    */

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        struct stat buf;
        char *ptr;

        struct stat buf3;
        if (stat(dir, &buf3) < 0)
            fprintf(stderr, "stat error:%s\n", strerror(errno));
        printf("文件长度:%d\n", buf3.st_size);
        printf("普通文件:%s\n", S_ISREG(buf3.st_mode) ? "T" : "F");
        printf("目录文件:%s\n", S_ISDIR(buf3.st_mode) ? "T" : "F");
        printf("字符特殊文件:%s\n", S_ISCHR(buf3.st_mode) ? "T" : "F");
        printf("块特殊文件:%s\n", S_ISBLK(buf3.st_mode) ? "T" : "F");
        printf("管道或FIFO:%s\n", S_ISFIFO(buf3.st_mode) ? "T" : "F");
        printf("符号链接:%s\n", S_ISLNK(buf3.st_mode) ? "T" : "F");
        printf("套接字:%s\n", S_ISSOCK(buf3.st_mode) ? "T" : "F");
        printf("消息队列:%s\n", S_TYPEISMQ(&buf3) ? "T" : "F");
        printf("信号量:%s\n", S_TYPEISSEM(&buf3) ? "T" : "F");
        printf("共享存储对象:%s\n", S_TYPEISSHM(&buf3) ? "T" : "F");
        printf("--------------------\n");

        if (lstat(dir, &buf) < 0)
            fprintf(stderr, "lstat error:%s\n", strerror(errno));
        printf("文件长度:%d\n", buf.st_size);
        printf("普通文件:%s\n", S_ISREG(buf.st_mode) ? "T" : "F");
        printf("目录文件:%s\n", S_ISDIR(buf.st_mode) ? "T" : "F");
        printf("字符特殊文件:%s\n", S_ISCHR(buf.st_mode) ? "T" : "F");
        printf("块特殊文件:%s\n", S_ISBLK(buf.st_mode) ? "T" : "F");
        printf("管道或FIFO:%s\n", S_ISFIFO(buf.st_mode) ? "T" : "F");
        printf("符号链接:%s\n", S_ISLNK(buf.st_mode) ? "T" : "F");
        printf("套接字:%s\n", S_ISSOCK(buf.st_mode) ? "T" : "F");
        printf("消息队列:%s\n", S_TYPEISMQ(&buf) ? "T" : "F");
        printf("信号量:%s\n", S_TYPEISSEM(&buf) ? "T" : "F");
        printf("共享存储对象:%s\n", S_TYPEISSHM(&buf) ? "T" : "F");
        printf("--------------------\n");

        struct stat buf2;
        if (fstat(fd, &buf2) < 0)
            fprintf(stderr, "fstat error:%s\n", strerror(errno));
        printf("文件长度:%d\n", buf2.st_size);
        printf("普通文件:%s\n", S_ISREG(buf2.st_mode) ? "T" : "F");
        printf("目录文件:%s\n", S_ISDIR(buf2.st_mode) ? "T" : "F");
        printf("字符特殊文件:%s\n", S_ISCHR(buf2.st_mode) ? "T" : "F");
        printf("块特殊文件:%s\n", S_ISBLK(buf2.st_mode) ? "T" : "F");
        printf("管道或FIFO:%s\n", S_ISFIFO(buf2.st_mode) ? "T" : "F");
        printf("符号链接:%s\n", S_ISLNK(buf2.st_mode) ? "T" : "F");
        printf("套接字:%s\n", S_ISSOCK(buf2.st_mode) ? "T" : "F");
        printf("消息队列:%s\n", S_TYPEISMQ(&buf2) ? "T" : "F");
        printf("信号量:%s\n", S_TYPEISSEM(&buf2) ? "T" : "F");
        printf("共享存储对象:%s\n", S_TYPEISSHM(&buf2) ? "T" : "F");
        printf("--------------------\n");

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

/** 文件权限 **/

文件有3类型拥有者:用户、组、其他
文件的每个拥有者有3类权限：读、写、执行
不同类型的权限可能会有不同的作用，规则如下：
1. 目录的读权限允许查看目录内所有的的文件名列表，目录的执行权限允许搜索文件名，目录的写权限和执行权限允许在目录中创建/删除文件
2. 读权限允许是否可以对文件执行读操作
3. 写权限允许是否可以对文件执行写操作
4. exec函数执行具有执行权限的普通文件

**使用access和facessat测试文件权限或是否存在**

```c
#include <unistd.h>
void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        printf("文件是否存在:%s\n", !access(dir, F_OK) ? "T" : "F");
        printf("文件是否可读:%s\n", !access(dir, R_OK) ? "T" : "F");
        printf("文件是否可写:%s\n", !access(dir, W_OK) ? "T" : "F");
        printf("文件是否可执行:%s\n", !access(dir, X_OK) ? "T" : "F");

        printf("文件是否存在:%s\n", !faccessat(AT_FDCWD, dir, F_OK, AT_EACCESS) ? "T" : "F");
        printf("操作该文件的进程(有效用户ID或有效组ID)是否有可读权限:%s\n", !faccessat(AT_FDCWD, dir, R_OK, AT_EACCESS) ? "T" : "F");
        printf("操作该文件的进程(有效用户ID或有效组ID)是否有可写权限:%s\n", !faccessat(AT_FDCWD, dir, W_OK, AT_EACCESS) ? "T" : "F");
        printf("操作该文件的进程(有效用户ID或有效组ID)是否有可执行权限:%s\n", !faccessat(AT_FDCWD, dir, X_OK, AT_EACCESS) ? "T" : "F");
        
        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**使用umask屏蔽设置的权限**

使用umask可以让系统屏蔽一些权限，让这些权限无法被设置，umask没有错误返回值

```c
#include <sys/stat.h>
void test(void)
{
    umask(0);

    char *dir1 = "file1", *dir2 = "file2";

    if (creat(dir1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));
    printf("文件是否存在:%s\n", !access(dir1, F_OK) ? "T" : "F");
    printf("文件是否可读:%s\n", !access(dir1, R_OK) ? "T" : "F");
    printf("文件是否可写:%s\n", !access(dir1, W_OK) ? "T" : "F");
    printf("文件是否可执行:%s\n", !access(dir1, X_OK) ? "T" : "F");

    /**
     * S_IRUSR S_IRGRP S_IROTH  拥有者/组/其他-读
     * S_IWUSR S_IWGRP S_IWOTH  拥有者/组/其他-写
     * S_IXUSR S_IXGRP S_IXOTH  拥有者/组/其他-执行
    */
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | S_IXUSR);

    if (creat(dir2, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    /* 只能测试拥有者的权限 */
    printf("文件是否存在:%s\n", !access(dir2, F_OK) ? "T" : "F");
    printf("文件是否可读:%s\n", !access(dir2, R_OK) ? "T" : "F");
    printf("文件是否可写:%s\n", !access(dir2, W_OK) ? "T" : "F");
    printf("文件是否可执行:%s\n", !access(dir2, X_OK) ? "T" : "F");
}
```

**修改文件的权限**

使用chmod、fchmod、fchmodat来设置权限

```c
#include <sys/stat.h>
void test(void)
{
    char *dir1 = "file1";

    int fd = open(dir1, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        printf("文件是否存在:%s\n", !access(dir1, F_OK) ? "T" : "F");
        printf("文件是否可读:%s\n", !access(dir1, R_OK) ? "T" : "F");
        printf("文件是否可写:%s\n", !access(dir1, W_OK) ? "T" : "F");
        printf("文件是否可执行:%s\n", !access(dir1, X_OK) ? "T" : "F");

        if (chmod(dir1, 0555) < 0)
            fprintf(stderr, "Close error:%s\n", strerror(errno));

        if (fchmod(fd, 0666) < 0)
            fprintf(stderr, "fchmod error:%s\n", strerror(errno));

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**修改用户ID和组ID**

使用chown、fchown、fchownat和lchown来修改用户ID和组ID

```c
#include <sys/stat.h>
void test(void)
{
    char *dir1 = "file1";

    int fd = open(dir1, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        // cat /etc/group查看组id
        // cat /etc/passwd查看用户id
        if (chown(dir1, 1000, 1000) < 0)
            fprintf(stderr, "Chown error:%s\n", strerror(errno));
        system("ls file1 -al");

        if (lchown(dir1, 1000, 1000) < 0)
            fprintf(stderr, "Chown error:%s\n", strerror(errno));
        system("ls file1 -al");

        if (fchown(fd, 1000, 1000) < 0)
            fprintf(stderr, "Chown error:%s\n", strerror(errno));    
        system("ls file1 -al");
        
        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}

```
**截断文件的内容**

使用truncate、ftruncate来截断文件的内容

```c
#include <sys/stat.h>
void test(void)
{
    char *dir = "file";

    int fd = open(dir, O_RDWR | O_CREAT, 0777);
    if (fd)
    {
        char *buf1 = "1234556";
        char buf2[sizeof(buf1)] = {0};

        if (write(fd, buf1, sizeof(buf1)) != sizeof(buf1))
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        
        if (pread(fd, buf2, sizeof(buf2), 0) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        buf2[sizeof(buf2) - 1] = 0;
        printf("buf2:%s\n", buf2);

        // truncate截断文件(但是这里暂时没有生效，需要用ftruncate)
        truncate(dir, 5);

        if (pread(fd, buf2, sizeof(buf2), 0) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        buf2[sizeof(buf2) - 1] = 0;
        printf("buf2:%s\n", buf2);

        // ftruncate截断文件
        ftruncate(fd, 6);

        if (pread(fd, buf2, sizeof(buf2), 0) < 0)
            fprintf(stderr, "lseek error:%s\n", strerror(errno));
        buf2[sizeof(buf2) - 1] = 0;
        printf("buf2:%s\n", buf2);

        if (close(fd))
            fprintf(stderr, "Close error:%s\n", strerror(errno));
    }
}
```

**硬链接和软链接**

硬链接一般不允许链接目录，且只能让同一文件系统间的文件链接。
软链接没有上述要求。

硬链接操作有:link、linkat、unlink
软链接操作有:symlink、symlinkat、unlink、readlink、readlinkat

```c
#include <unistd.h>
void test(void)
{
    char *dir1 = "file1", *dir1_link = "file1_link";
    char *dir2 = "file2", *dir2_symlink = "file2_symlink";

    if (creat(dir1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));
    if (creat(dir2, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    // 硬链接
    if (link(dir1, dir1_link) < 0)
        fprintf(stderr, "Link error:%s\n", strerror(errno));

    // 软链接
    if (symlink(dir2, dir2_symlink) < 0)
        fprintf(stderr, "Link error:%s\n", strerror(errno));

    // 读软链接
    char buf[100];
    if (readlink(dir2_symlink, buf, sizeof(buf)) < 0)
        fprintf(stderr, "Readlink error:%s\n", strerror(errno));
    printf("Readlink res:%s\n", buf);

    struct stat st;
    lstat(dir1, &st);
    printf("Link number:%ld\n", st.st_nlink);
    lstat(dir2, &st);
    printf("Link number:%ld\n", st.st_nlink);

    // 删除硬链接
    // unlink(dir1_link);
    remove(dir1_link);
    // 删除软链接
    unlink(dir2_symlink);

    lstat(dir1, &st);
    printf("Link number:%ld\n", st.st_nlink);
    lstat(dir2, &st);
    printf("Link number:%ld\n", st.st_nlink);
}
```

**文件重命名**

使用rename、renameat来修改文件名

```c
#include <unistd.h>
void test(void)
{
    char *dir1 = "file1", *dir1_rename = "file1_rename";

    if (creat(dir1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    // 重命名
    if (rename(dir1, dir1_rename) < 0)
        fprintf(stderr, "Rename error:%s\n", strerror(errno));

    if (!access(dir1, F_OK))
        printf("%s is exist!\n", dir1);
    else
        printf("%s is not exist!\n", dir1);

    if (!access(dir1_rename, F_OK))
        printf("%s is exist!\n", dir1_rename);
    else
        printf("%s is not exist!\n", dir1_rename);

    if (remove(dir1_rename) < 0)
        fprintf(stderr, "Remove error:%s\n", strerror(errno));
}
```

**文件时间的修改**

文件时间分为访问时间、修改时间，使用utimes(Linux不支持这个API)、ftimes、utimesat函数来更改这个时间。此外可以设置tv_nsec字段为UTIME_NOW或UTIME_OMIT来选择设置时间为当前时间、或不设置时间。

```c
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
```

**创建、删除目录**

使用mkdir、mkdirat创建目录，使用rmdir删除空目录
使用opendir、fdopendir打开目录
使用readdir读取目录
使用closedir关闭目录
使用rewinddir、seekdir、telldir操作目录指针位置

```c
#include <sys/types.h>
#include <dirent.h>

void test(void)
{
    char *dir1 = "file1_dir";
    char *file1 = "file1";

    if (mkdir(dir1, 0777) < 0)
        fprintf(stderr, "Mkdir error:%s\n", strerror(errno));
    
    /* 打开目录 */
    DIR *open_dir = NULL;
    struct dirent *dirp;
    open_dir = opendir(dir1);
    if (open_dir)
    {
        /* 查询目录 */
        rewinddir(open_dir);        // 目录指针恢复到起始位置
        seekdir(open_dir, 5830436974003520366);      // 修改目录指针
        while ((dirp = readdir(open_dir)) != NULL)
        if (dirp)
        {
            printf("目录指针:%ld\t", telldir(open_dir));
            printf("inode:%ld\t", dirp->d_ino);
            printf("名字:%s\t", dirp->d_name);
            printf("偏移:%d\t", dirp->d_reclen);
            printf("记录长度:%d\t", dirp->d_reclen);
            printf("类型:");
            switch (dirp->d_type)
            {
                case DT_BLK:printf("block dev\n");break;
                case DT_CHR:printf("character dev\n");break;
                case DT_DIR:printf("directory\n");break;
                case DT_FIFO:printf("pipe(fifo)\n");break;
                case DT_LNK:printf("symlink\n");break;
                case DT_REG:printf("regular file\n");break;
                case DT_SOCK:printf("socket\n");break;
                case DT_UNKNOWN:printf("unknown\n");break;
                default:printf("type is unknown\n");break;
            }
        }
        /* 关闭目录 */
        if (closedir(open_dir) < 0)
            fprintf(stderr, "Closedir error:%s\n", strerror(errno));
    }

    if (creat(file1, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    if (rmdir(dir1) < 0)
        fprintf(stderr, "Rmdir error:%s\n", strerror(errno));
        
    if (!access(file1, F_OK))
    {
        if (remove(file1) < 0)
            fprintf(stderr, "Remove error:%s\n", strerror(errno));
    }

    if (!access(dir1, F_OK))
    {
        if (remove(dir1) < 0)
            fprintf(stderr, "Remove error:%s\n", strerror(errno));
    }
}
```

**更改/设置当前工作目录**

使用fchdir、chdir设置当前工作目录，使用getcwd获取当前工作目录

```c
#include <unistd.h>

void test(void)
{
    if (chdir("/tmp") < 0)
        fprintf(stderr, "Chdir error:%s\n", strerror(errno));
    
    char buf[200];
    if (getcwd(buf, sizeof(buf)))
    {
        printf("getcwd:%s\n", buf);
    }
}
```

**获取主次设备号**

```c
#include <sys/stat.h>
#include <sys/sysmacros.h>
void test(void)
{
    char *dir = "file";
    if (creat(dir, 0777) < 0)
        fprintf(stderr, "Creat error:%s\n", strerror(errno));

    struct stat buf;
    if (lstat(dir, &buf) < 0)
        fprintf(stderr, "Lstat error:%s\n", strerror(errno));

    /* 打印主次设备号 */
    printf("dev = %d/%d\n", major(buf.st_dev), minor(buf.st_dev));

    /* 打印字符/块设备实际的主/次设备号 */
    if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode))
    {
        printf("%s rdev = %d/%d\n", 
        S_ISCHR(buf.st_mode) ? "character" : "block", 
        major(buf.st_rdev), 
        minor(buf.st_rdev));
    }
    
    remove(dir);
}
```

## 系统数据文件和信息

Linux运行过程会伴随大量与系统有关的数据文件

**口令文件**

由于历史的原因，Linux口令文件是/etc/passwd

```c
/* A record in the user database.  */
struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Hashed passphrase, if shadow database
                                   not in use (see shadow.h).  */
  __uid_t pw_uid;		/* User ID.  */
  __gid_t pw_gid;		/* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};
```

1. 如果想要阻止用户登录，可以将Shell程序设置为/dev/null、/bin/false(返回失败)或/bin/true(返回成功)
2. nobody用户id为65534，组id为65534，不提供任何权限，目的是让任何人都可以登录到系统。

**阴影口令文件**

阴影口令文件是单向加密算法处理过的用户口令副本，文件路径为/etc/shadow。为了防止外部用明文口令生成加密口令，再不断与其用户的加密口令对比，最终比对成功后得到原始口令，阴影口令一般只能由root用户访问，也因此普通口令文件/etc/passwd可以由各个用户读取。

```c
/* A record in the shadow database.  */
struct spwd
  {
    char *sp_namp;		/* Login name.  */
    char *sp_pwdp;		/* Hashed passphrase.  */
    long int sp_lstchg;		/* Date of last change.  */
    long int sp_min;		/* Minimum number of days between changes.  */
    long int sp_max;		/* Maximum number of days between changes.  */
    long int sp_warn;		/* Number of days to warn user to change
				   the password.  */
    long int sp_inact;		/* Number of days the account may be
				   inactive.  */
    long int sp_expire;		/* Number of days since 1970-01-01 until
				   account expires.  */
    unsigned long int sp_flag;	/* Reserved.  */
  };
```

**组文件**

组文件/etc/group

```c
/* The group structure.	 */
struct group
  {
    char *gr_name;		/* Group name.	*/
    char *gr_passwd;	/* Password.	*/
    __gid_t gr_gid;		/* Group ID.	*/
    char **gr_mem;		/* Member list.	指向各用户指针的数组*/
  };
```
**附属组id**

每个用户除了拥有一个组以外，也可以拥有多个附属组。这是为了解决一个用户参与多个项目时，需要同时属于多个组。

**其他数据文件**

`/etc/nsswwitch.conf`，系统可以使用网络信息服务(NIS)来管理用户和组的数据库。系统通过配置文件/etc/nsswitch.conf来管理每一类信息

`/etc/protocols`,记录各网络服务器提供服务的数据文件

`/etc/protocols`,记录协议信息的数据文件

`/etc/networks`，记录网络信息的数据文件

**登录账户记录**

有两个文件来记录账户信息：
1. `/var/run/utmp`:记录当前登录到系统的用户
2. `/var/log/wtmp`:记录用户登录的注销的日志
```c
/* The structure describing an entry in the user accounting database.  */
struct utmp
{
  short int ut_type;		/* Type of login.  */
  pid_t ut_pid;			/* Process ID of login process.  */
  char ut_line[UT_LINESIZE];	/* Devicename.  */
  char ut_id[4];		/* Inittab ID.  */
  char ut_user[UT_NAMESIZE];	/* Username.  */
  char ut_host[UT_HOSTSIZE];	/* Hostname for remote login.  */
  struct exit_status ut_exit;	/* Exit status of a process marked
				   as DEAD_PROCESS.  */
/* The ut_session and ut_tv fields must be the same size when compiled
   32- and 64-bit.  This allows data files and shared memory to be
   shared between 32- and 64-bit applications.  */
#ifdef __WORDSIZE_TIME64_COMPAT32
  int32_t ut_session;		/* Session ID, used for windowing.  */
  struct
  {
    int32_t tv_sec;		/* Seconds.  */
    int32_t tv_usec;		/* Microseconds.  */
  } ut_tv;			/* Time entry was made.  */
#else
  long int ut_session;		/* Session ID, used for windowing.  */
  struct timeval ut_tv;		/* Time entry was made.  */
#endif

  int32_t ut_addr_v6[4];	/* Internet address of remote host.  */
  char __glibc_reserved[20];		/* Reserved for future use.  */
};
```

**系统标识**

一般情况使用uname来获取主机和操作系统的有关信息

struct utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
#ifdef _GNU_SOURCE
	char domainname[65];
#else
	char __domainname[65];
#endif
};

**时间和日期**

有一系列的函数来获取和设置时间：

```c
time_t time(time_t *calptr);

/**
 * CLOCK_REALTIME   实时系统时间
 * CLOCK_MONOTONIC  不带负跳数的实时系统时间
 * CLOCK_PROCESS_CPUTIME_ID 调用进程的CPU时间
 * CLOCK_THREAD_CPUTIME_ID  调用线程的CPU时间
*/
int clock_gettime(clockid_t clock_id, struct timespec *tsp);
int clock_getres(clockid_t clock_id, struct timespec *tsp);
int clock_settime(clockid_t clock_id, const struct timespec *tsp);

/**
 * 第二个参数只能是NULL
*/
int gettimeofday(struct timeval *restrict tp, void *restrict tzp);

/* 时间格式转换 */
struct tm *gmtime(const time_t *calptr);
struct tm *localtime(const time_t *calptr);
time_t mktime(struct tm *tmptr);

/* 时间格式打印 */
size_t strftime(char *dst, size_t dst_size, const char *fmt, const struct tm *tm);
size_t strftime_l (char *__restrict __s, size_t __maxsize,
			  const char *__restrict __format,
			  const struct tm *__restrict __tp,
			  __locale_t __loc);
char *strptime (const char *__restrict __s,const char *__restrict __fmt, struct tm *__tp);
```

## 其他

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
#### 用户标识

/etc/group  分组信息

```c
#include <unistd.h>
#include <sys/types.h>
void test(void)
{
    printf("uid:%d  gid:%d\n", getuid(), getgid()); 
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

#### 时间值

两种时间值：日历时间和进程时间

1. 日历时间
   UTC格式的世界时间,一般用time_t格式保存
2. 进程时间
   进程时间又称CPU时间，用来度量进程占用CPU资源的情况。以时钟滴答计算，一般用clock_t保存。
   进程时间又分为3类,可以使用`time`命令得到：
   - 时钟时间
   - 用户CPU时间：该进程执行用户指令使用的时间量
   - 系统CPU时间：该进程执行内核程序使用的时间量。例如read或write会调用内核程序。
  


#### 查看一些限制参数

Linux提供了一些宏来指示对参数的限制值，可以通过`sysconf`,`pathconf`,`fpathconf`查看。但注意某些限制值会在运行过程中改变，例如`_SC_OPEN_MAX`和`_PC_PATH_MAX`。如果要修改限制，可以用`ulimit`、`setrlimit`、`getrlimit`等函数修改，但是因为不常用就先不深究。

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void test(void)
{
    /* 查看系统的限制参数 */
    printf("Version:%ld\n", sysconf(_SC_VERSION));
    printf("XOPEN Version:%ld\n", sysconf(_SC_XOPEN_VERSION));
    printf("The size of page in bytes:%ld\n", sysconf(_SC_PAGE_SIZE));
    printf("Open file maximum num:%ld\n", sysconf(_SC_OPEN_MAX));
    printf("The number of clock ticks per second:%ld\n", sysconf(_SC_CLK_TCK));
    printf("ARG_MAX:%ld\n", sysconf(_SC_ARG_MAX));

    /* 查看某路径的限制参数 */
    char *dir = "/home/lxo";
    printf("_PC_NAME_MAX with %s :%ld\n", dir, pathconf(dir, _PC_NAME_MAX));
    printf("_PC_PATH_MAX with %s :%ld\n", dir, pathconf(dir, _PC_PATH_MAX));
    printf("_PC_LINK_MAX with %s :%ld\n", dir, pathconf(dir, _PC_LINK_MAX));
    printf("_PC_MAX_INPUT with %s :%ld\n", dir, pathconf(dir, _PC_MAX_INPUT));
    printf("_PC_MAX_CANON with %s :%ld\n", dir, pathconf(dir, _PC_MAX_CANON));
    printf("_PC_PIPE_BUF with %s :%ld\n", dir, pathconf(dir, _PC_PIPE_BUF));

    /* 查看某文件的限制参数 */
    int fd = open(dir, O_RDONLY);
    if (fd > 0)
    {
        printf("_PC_NAME_MAX :%ld\n", fpathconf(fd, _PC_NAME_MAX));
        printf("_PC_PATH_MAX :%ld\n", fpathconf(fd, _PC_PATH_MAX));
        printf("_PC_LINK_MAX :%ld\n", fpathconf(fd, _PC_LINK_MAX));
        printf("_PC_MAX_INPUT :%ld\n", fpathconf(fd, _PC_MAX_INPUT));
        printf("_PC_MAX_CANON :%ld\n", fpathconf(fd, _PC_MAX_CANON));
        printf("_PC_PIPE_BUF :%ld\n", fpathconf(fd, _PC_PIPE_BUF));

        close(fd);
    }
}
```